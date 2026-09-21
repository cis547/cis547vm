#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace instrument {

using CoverageSignature = std::vector<std::string>;

// Provided helper: normalize a coverage profile without changing entry bytes.
CoverageSignature makeCoverageSignature(CoverageSignature entries);

// Apply FAST Equation (4) with the course parameters specified in the README.
// Every uint64_t input is valid.
uint32_t assignEnergy(uint64_t priorSelections, uint64_t pathFrequency);

class PowerSchedule {
 public:
  // Register exact seed bytes once, keeping the first signature. Registration
  // does not count an execution or change an existing seed's selection count.
  void registerSeed(const std::string& seed, const CoverageSignature& signature);

  // Count every execution, including executions whose inputs are not retained.
  // Seeds sharing a signature share this frequency; counts saturate at UINT64_MAX.
  void observeExecution(const CoverageSignature& signature);

  // Compute energy from the prior count, then count one selection. An unknown
  // seed returns zero without registering it. Selection counts also saturate.
  uint32_t nextEnergy(const std::string& seed);

  // Unknown seeds and signatures return zero without changing state.
  uint64_t selectionCount(const std::string& seed) const;
  uint64_t pathFrequency(const CoverageSignature& signature) const;

 private:
  struct SeedState {
    CoverageSignature signature;
    uint64_t selections = 0;
  };

  std::map<std::string, SeedState> seeds;
  std::map<CoverageSignature, uint64_t> frequencies;
};

}  // namespace instrument
