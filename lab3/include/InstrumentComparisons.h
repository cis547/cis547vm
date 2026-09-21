#pragma once

#include "llvm/IR/PassManager.h"

namespace instrument {

class InstrumentComparisons : public llvm::PassInfoMixin<InstrumentComparisons> {
 public:
  llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager& AM);
};

}  // namespace instrument
