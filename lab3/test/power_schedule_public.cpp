#include "PowerSchedule.h"

#include <iostream>

int main() {
  using instrument::assignEnergy;
  using instrument::CoverageSignature;
  using instrument::makeCoverageSignature;
  using instrument::PowerSchedule;

  bool passed = true;
  auto check = [&passed](const char* name, bool condition) {
    if (!condition) {
      std::cerr << name << " failed\n";
      passed = false;
    }
  };

  check("first selection", assignEnergy(0, 1) == 16);
  check("unobserved profile", assignEnergy(0, 0) == 16);
  check("repeat selection", assignEnergy(1, 1) == 32);
  check("frequent profile", assignEnergy(2, 8) == 8);
  check("integer rounding", assignEnergy(0, 3) == 5);
  check("minimum energy", assignEnergy(0, 100) == 1);
  check("maximum energy", assignEnergy(10, 1) == 64);

  const std::string binaryEntry("x\0y", 3);
  const CoverageSignature profile = {"20, 1", "10, 2", "20, 1"};
  const CoverageSignature canonical = {"10, 2", "20, 1"};
  check("signature normalization", makeCoverageSignature(profile) == canonical);
  check("signature preserves raw bytes",
      makeCoverageSignature({"", "x", binaryEntry, binaryEntry}) ==
          CoverageSignature({"", "x", binaryEntry}));

  PowerSchedule schedule;
  check("unknown seed", schedule.nextEnergy("missing") == 0);
  check("unknown selection count", schedule.selectionCount("missing") == 0);
  check("unknown profile", schedule.pathFrequency(profile) == 0);
  schedule.observeExecution(profile);
  schedule.observeExecution(canonical);
  schedule.registerSeed("first", canonical);
  schedule.registerSeed("second", profile);
  check("registration does not observe execution", schedule.pathFrequency(profile) == 2);
  check("initial selection count", schedule.selectionCount("first") == 0);
  check("first shared-profile energy", schedule.nextEnergy("first") == 8);
  check("selection counted once", schedule.selectionCount("first") == 1);
  check("selection does not observe execution", schedule.pathFrequency(profile) == 2);
  check("separate seed selection counts", schedule.nextEnergy("second") == 8);
  check("repeat energy", schedule.nextEnergy("first") == 16);
  schedule.observeExecution(canonical);
  check("all executions count", schedule.pathFrequency(profile) == 3);
  schedule.registerSeed("first", {"different profile"});
  check("duplicate seed keeps count", schedule.selectionCount("first") == 2);
  check("duplicate seed keeps original profile", schedule.nextEnergy("first") == 21);
  check("duplicate registration does not create profile",
      schedule.pathFrequency({"different profile"}) == 0);

  const std::string binarySeed("a\0b", 3);
  schedule.registerSeed("a", {});
  schedule.registerSeed(binarySeed, {});
  schedule.registerSeed("", {});
  check("binary seed", schedule.nextEnergy(binarySeed) == 16);
  check("binary seed distinct from prefix", schedule.selectionCount("a") == 0);
  check("empty seed", schedule.nextEnergy("") == 16);
  check("empty profile is valid", schedule.pathFrequency({}) == 0);
  schedule.observeExecution({});
  check("empty profile execution", schedule.pathFrequency({}) == 1);

  if (passed) {
    std::cout << "Power schedule checks passed\n";
  }
  return passed ? 0 : 1;
}
