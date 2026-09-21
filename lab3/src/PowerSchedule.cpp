#include "PowerSchedule.h"

#include <algorithm>
#include <limits>

namespace instrument {

CoverageSignature makeCoverageSignature(CoverageSignature entries) {
  std::sort(entries.begin(), entries.end());
  entries.erase(std::unique(entries.begin(), entries.end()), entries.end());
  return entries;
}

uint32_t assignEnergy(uint64_t priorSelections, uint64_t pathFrequency) {
  // TODO: Compute this seed's energy from the FAST schedule.
  return 1;
}

void PowerSchedule::registerSeed(
    const std::string& seed, const CoverageSignature& signature) {
  // TODO: Implement.
}

void PowerSchedule::observeExecution(const CoverageSignature& signature) {
  // TODO: Implement.
}

uint32_t PowerSchedule::nextEnergy(const std::string& seed) {
  // TODO: Compute this seed's energy, then advance its selection count.
  return 1;
}

uint64_t PowerSchedule::selectionCount(const std::string& seed) const {
  // TODO: Implement.
  return 0;
}

uint64_t PowerSchedule::pathFrequency(const CoverageSignature& signature) const {
  // TODO: Implement.
  return 0;
}

}  // namespace instrument
