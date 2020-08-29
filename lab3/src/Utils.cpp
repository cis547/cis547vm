# include <Utils.h>

int successCount = 1;
int failureCount = 1;

std::string readOneFile(std::string &Path) {
  std::ifstream SeedFile(Path);
  std::string Line((std::istreambuf_iterator<char>(SeedFile)),
                   std::istreambuf_iterator<char>());
  return Line;
}

int runTarget(std::string &Target, std::string &Input) {
  std::string Cmd = Target + " > /dev/null 2>&1";
  FILE *F = popen(Cmd.c_str(), "w");
  fprintf(F, "%s", Input.c_str());
  return pclose(F);
}

void initialize(std::string &OutDir) {
  int Status;
  std::string SuccessDir = OutDir + "/success";
  std::string FailureDir = OutDir + "/failure";
  mkdir(SuccessDir.c_str(), 0755);
  mkdir(FailureDir.c_str(), 0755);
}

void storePassingInput(std::string &Input, std::string &OutDir) {
  std::string Path = OutDir + "/success/input" + std::to_string(successCount++);
  std::ofstream OutFile(Path);
  OutFile << Input;
  OutFile.close();
}

void storeCrashingInput(std::string &Input, std::string &OutDir) {
  std::string Path = OutDir + "/failure/input" + std::to_string(failureCount++);
  std::ofstream OutFile(Path);
  OutFile << Input;
  OutFile.close();
}
