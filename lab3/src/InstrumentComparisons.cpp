#include "InstrumentComparisons.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

namespace instrument {

PreservedAnalyses InstrumentComparisons::run(Module& M, ModuleAnalysisManager& AM) {
  // TODO: Insert calls to the provided runtime hooks before eligible
  // comparisons. See the handout for the required scope and log format.
  return PreservedAnalyses::all();
}

}  // namespace instrument
