#include "BuildDictionary.h"

#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace llvm;

#define MAX_ENTRY_SIZE 1024  // Reasonable size limit

namespace instrument {

void BuildDictionaryPass::addToDictionary(const std::string &value) {
  if (!value.empty() && value.size() <= MAX_ENTRY_SIZE) {
    dictionary.insert({value, INT_MAX});
  }
}

void BuildDictionaryPass::addToDictionaryWithHint(
    const std::string &value, int positionHint) {
  if (!value.empty() && value.size() <= MAX_ENTRY_SIZE) {
    dictionary.insert({value, positionHint});
  }
}

// Extract string literals from global variables
void BuildDictionaryPass::extractFromGlobalVariable(GlobalVariable *GV) {
  if (!GV->hasInitializer())
    return;

  Constant *initializer = GV->getInitializer();
  extractFromConstant(initializer);
}

// Extract constants from various constant types
void BuildDictionaryPass::extractFromConstant(Constant *C) {
  if (ConstantDataArray *CDA = dyn_cast<ConstantDataArray>(C)) {
    // TODO: Extract string constants and call `addToDictionary`
  } else if (ConstantInt *CI = dyn_cast<ConstantInt>(C)) {
    // TODO: Extract integer constants and call `addToDictionary`
  }
}

// Extract constants from function instructions
void BuildDictionaryPass::extractFromFunction(Function &F) {
  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      if (ICmpInst *ICI = dyn_cast<ICmpInst>(&I)) {
        // Extract comparison constants
        for (unsigned i = 0; i < ICI->getNumOperands(); ++i) {
          if (ConstantInt *CI = dyn_cast<ConstantInt>(ICI->getOperand(i))) {
            extractFromConstant(CI);
          }
        }
      }
    }
  }
}

// Write dictionary to file
void BuildDictionaryPass::writeDictionary(Module &M) {
  std::string outputDir = "fuzzing_dict";

  // Create directory if it doesn't exist
  std::system(("mkdir -p " + outputDir).c_str());

  size_t entryNum = 0;
  for (const auto &entry : dictionary) {
    std::string outputFile = outputDir + "/entry_" + std::to_string(entryNum);
    if (entry.second != INT_MAX) {
      outputFile += "@" + std::to_string(entry.second);
    }

    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
      errs() << "Error: Could not open output file " << outputFile << "\n";
      continue;
    }

    // Write string directly
    outFile << entry.first;

    outFile.close();
    entryNum++;
  }

  outs() << "Fuzzing dictionary written to " << outputDir << " with " << dictionary.size()
         << " entries\n";
}

PreservedAnalyses BuildDictionaryPass::run(Module &M, ModuleAnalysisManager &AM) {
  outs() << "Running BuildDictionaryPass on module: " << M.getName() << "\n";

  // Extract from global variables
  for (GlobalVariable &GV : M.globals()) {
    extractFromGlobalVariable(&GV);
  }

  // Extract from functions
  for (Function &F : M) {
    if (!F.isDeclaration()) {
      extractFromFunction(F);
    }
  }

  // Write the collected dictionary
  writeDictionary(M);

  return PreservedAnalyses::all();
}

}  // namespace instrument
