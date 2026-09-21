#include "DictionaryMutation.h"

#include <iostream>
#include <limits>

int main() {
  using instrument::applyDictionaryEntry;
  bool Passed = true;
  auto Check = [&Passed](const char* Name,
                   const std::string& Actual,
                   const std::string& Expected) {
    if (Actual != Expected) {
      std::cerr << Name << " failed\n";
      Passed = false;
    }
  };
  Check("insertion",
      applyDictionaryEntry("abcd", {"XY", std::nullopt}, 2, false),
      "abXYcd");
  Check("overwrite",
      applyDictionaryEntry("abcd", {"XYZ", std::nullopt}, 3, true),
      "abcXYZ");
  Check("binary token",
      applyDictionaryEntry("ab", {std::string("X\0Y", 3), std::nullopt}, 1, false),
      std::string("aX\0Yb", 5));
  Check(
      "hint overrides offset", applyDictionaryEntry("abcd", {"XY", 1}, 0, true), "aXYd");
  Check("hint padding",
      applyDictionaryEntry("a", {"Z", 3}, 0, false),
      std::string("a\0\0Z", 4));
  Check("unhinted gap", applyDictionaryEntry("ab", {"Z", std::nullopt}, 3, false), "ab");
  Check("oversized hint",
      applyDictionaryEntry("ab", {"Z", std::numeric_limits<uint64_t>::max()}, 0, true),
      "ab");
  if (Passed) {
    std::cout << "Dictionary application checks passed\n";
  }
  return Passed ? 0 : 1;
}
