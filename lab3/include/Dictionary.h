#pragma once

#include <cstdint>
#include <optional>
#include <set>
#include <string>

namespace instrument {

struct DictionaryEntry {
  std::string bytes;
  std::optional<uint64_t> positionHint;

  bool operator<(const DictionaryEntry& other) const {
    if (bytes != other.bytes) {
      return bytes < other.bytes;
    }
    return positionHint < other.positionHint;
  }
};

using Dictionary = std::set<DictionaryEntry>;

}  // namespace instrument
