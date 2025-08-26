#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace instrument {

struct StaticAnalysisPass : public PassInfoMixin<StaticAnalysisPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

struct DynamicAnalysisPass : public PassInfoMixin<DynamicAnalysisPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

}  // namespace instrument
