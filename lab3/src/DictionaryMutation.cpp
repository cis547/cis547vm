#include "DictionaryMutation.h"

#include <algorithm>

namespace instrument {

std::string applyDictionaryEntry(
    std::string Input, const DictionaryEntry& Entry, size_t Offset, bool Overwrite) {
  // TODO: Apply this dictionary entry to Input, following the handout contract.
  // For entries loaded from disk, Entry.positionHint is the filename's @offset
  // suffix. Offset is the caller-selected fallback used only when the hint is absent.
  return Input;
}

}  // namespace instrument
