#pragma once

#include "Dictionary.h"

#include <cstddef>
#include <string>

namespace instrument {

inline constexpr size_t MaxDictionaryInputSize = 64 * 1024;

// Apply one raw-byte token. A position hint overrides Offset and permits
// zero-padding up to that position. Unhinted offsets must be within Input or
// exactly at its end. Overwrite replaces up to the token length and can extend
// Input; insertion preserves its suffix. Invalid entries, positions, or sizes
// return Input unchanged. See the README for the complete size contract.
std::string applyDictionaryEntry(
    std::string Input, const DictionaryEntry& Entry, size_t Offset, bool Overwrite);

}  // namespace instrument
