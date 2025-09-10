#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"

using namespace llvm;

namespace instrument {

struct InstrumentDivision : public PassInfoMixin<InstrumentDivision> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

}  // namespace instrument
