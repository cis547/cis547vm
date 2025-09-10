#pragma once

#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#include <cstddef>
#include <set>
#include <string>
#include <vector>

using namespace llvm;

namespace instrument {

std::string getDictionaryFilename(Module &M);

class BuildDictionaryPass : public PassInfoMixin<BuildDictionaryPass> {
 private:
  std::set<std::pair<std::string, int>> dictionary;

  void addToDictionary(const std::string &value);
  void addToDictionaryWithHint(const std::string &value, int positionHint);
  void writeDictionary(Module &M);

  void extractFromGlobalVariable(GlobalVariable *GV);
  void extractFromConstant(Constant *C);
  void extractFromFunction(Function &F);

 public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

}  // namespace instrument
