#pragma once

#include <cstdint>
#include <optional>
#include <set>
#include <string>

namespace instrument {

struct DictionaryEntry {
  std::string bytes;
  // Optional zero-based byte offset in the input, serialized as @offset in
  // entry_N@offset. A hint of zero is distinct from an absent hint.
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
