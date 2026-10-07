#pragma once

#include "Dictionary.h"

#include <cstddef>
#include <string>

namespace instrument {

inline constexpr size_t MaxDictionaryInputSize = 64 * 1024;

// Apply one raw-byte token. Offset is the caller-selected zero-based input byte
// offset, used only when Entry.positionHint is absent. A present hint, including
// zero, overrides Offset and permits zero-padding up to that position. A strategy
// may pass an entry copy with its hint cleared to use Offset instead. Unhinted
// offsets must be within Input or exactly at its end. Overwrite replaces up to
// the token length and can extend Input. Insertion preserves its suffix. Invalid
// entries, positions, or sizes return Input unchanged. See the README for the
// complete size contract.
std::string applyDictionaryEntry(
    std::string Input, const DictionaryEntry& Entry, size_t Offset, bool Overwrite);

}  // namespace instrument
