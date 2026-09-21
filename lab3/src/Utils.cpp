#include <sys/stat.h>

#include <Utils.h>
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

int successCount = 0;
int failureCount = 0;

void initialize(std::string& OutDir) {
  std::string SuccessDir = OutDir + "/success";
  mkdir(SuccessDir.c_str(), 0755);

  std::string FailureDir = OutDir + "/failure";
  mkdir(FailureDir.c_str(), 0755);
}

std::string readOneFile(std::string& Path) {
  std::ifstream SeedFile(Path, std::ios::binary);
  std::string Line(
      (std::istreambuf_iterator<char>(SeedFile)), std::istreambuf_iterator<char>());
  return Line;
}

int readSeedInputs(std::vector<std::string>& SeedInputs, std::string& SeedInputDir) {
  DIR* Directory;
  struct dirent* Ent;
  if ((Directory = opendir(SeedInputDir.c_str())) != NULL) {
    std::vector<std::string> Paths;
    while ((Ent = readdir(Directory)) != NULL) {
      if (!(Ent->d_type == DT_REG))
        continue;
      Paths.push_back(SeedInputDir + "/" + std::string(Ent->d_name));
    }
    closedir(Directory);

    std::sort(Paths.begin(), Paths.end());
    for (std::string& Path : Paths) {
      std::string Line = readOneFile(Path);
      SeedInputs.push_back(Line);
    }
    return 0;
  } else {
    return 1;
  }
}

void readCoverageFile(std::string& Target, std::vector<std::string>& CoverageData) {
  std::string CoveragePath = Target + ".cov";
  std::ifstream InFile(CoveragePath);
  std::string Line;
  while (std::getline(InFile, Line)) {
    CoverageData.push_back(Line);
  }
}

void storeSeed(std::string& OutDir, int randomSeed) {
  std::string Path = OutDir + "/randomSeed.txt";
  std::fstream File(Path, std::fstream::out | std::ios_base::trunc);
  File << std::to_string(randomSeed);
  File.close();
}

void storePassingInput(std::string& Input, std::string& OutDir) {
  std::string Path = OutDir + "/success/input" + std::to_string(successCount++);
  std::ofstream OutFile(Path, std::ios::binary | std::ios::trunc);
  OutFile.write(Input.data(), static_cast<std::streamsize>(Input.size()));
  OutFile.close();
}

void storeCrashingInput(std::string& Input, std::string& OutDir) {
  std::string Path = OutDir + "/failure/input" + std::to_string(failureCount++);
  std::ofstream OutFile(Path, std::ios::binary | std::ios::trunc);
  OutFile.write(Input.data(), static_cast<std::streamsize>(Input.size()));
  OutFile.close();
}

int runTarget(std::string& Target, std::string& Input) {
  std::string Cmd = Target + " > /dev/null 2>&1";
  FILE* F = popen(Cmd.c_str(), "w");
  // TODO: Make target execution binary-safe. C-string output truncates
  // inputs at their first NUL.
  fprintf(F, "%s", Input.c_str());
  return pclose(F);
}

std::optional<instrument::DictionaryEntry> readDictionaryEntry(
    const std::string& filename, const std::string& filepath) {
  constexpr uintmax_t MaxEntryBytes = 1024;
  std::error_code error;
  uintmax_t fileSize = std::filesystem::file_size(filepath, error);
  if (error || fileSize == 0 || fileSize > MaxEntryBytes) {
    return std::nullopt;
  }

  std::ifstream InFile(filepath, std::ios::binary);
  if (!InFile.is_open()) {
    return std::nullopt;
  }

  std::string result;
  result.reserve(static_cast<size_t>(fileSize));
  char byte;
  while (InFile.get(byte)) {
    result.push_back(byte);
  }

  std::optional<uint64_t> hint;
  size_t atPos = filename.find_last_of('@');
  size_t indexEnd = atPos == std::string::npos ? filename.size() : atPos;
  if (filename.rfind("entry_", 0) != 0 || indexEnd == 6) {
    return std::nullopt;
  }
  for (size_t index = 6; index < indexEnd; ++index) {
    if (!std::isdigit(static_cast<unsigned char>(filename[index]))) {
      return std::nullopt;
    }
  }
  if (atPos != std::string::npos) {
    if (atPos + 1 == filename.size()) {
      return std::nullopt;
    }
    uint64_t parsedHint = 0;
    const char* begin = filename.data() + atPos + 1;
    const char* end = filename.data() + filename.size();
    auto [position, status] = std::from_chars(begin, end, parsedHint);
    if (status != std::errc() || position != end) {
      return std::nullopt;
    }
    hint = parsedHint;
  }

  return instrument::DictionaryEntry{result, hint};
}

instrument::Dictionary readDictionary(std::string folder) {
  constexpr size_t MaxDictionaryFiles = 4096;
  constexpr uintmax_t MaxDictionaryBytes = 4 * 1024 * 1024;
  instrument::Dictionary result;
  size_t entriesSeen = 0;
  uintmax_t bytesRead = 0;

  if (!std::filesystem::exists(folder)) {
    std::filesystem::create_directories(folder);
  }

  for (auto const& ent : std::filesystem::directory_iterator(folder)) {
    if (entriesSeen++ == MaxDictionaryFiles) {
      break;
    }
    if (ent.is_symlink() || !ent.is_regular_file()) {
      continue;
    }

    const std::string name = ent.path().filename().string();
    if (name.size() >= 6 && name.compare(0, 6, "entry_") == 0) {
      std::optional<instrument::DictionaryEntry> entry =
          readDictionaryEntry(name, ent.path().string());
      if (!entry.has_value() || entry->bytes.size() > MaxDictionaryBytes - bytesRead) {
        continue;
      }
      bytesRead += entry->bytes.size();
      std::cout << "Dictionary: Loaded " << ent.path() << '\n';
      result.insert(std::move(*entry));
    }
  }

  return result;
}
