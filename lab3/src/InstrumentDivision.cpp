#include "InstrumentDivision.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugLoc.h"
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
#include "llvm/Support/raw_ostream.h"

#include <vector>

using namespace llvm;

namespace instrument {

static const auto PASS_DESC = "Instrumentation for Division";
static const char* SANITIZE_FUNCTION_NAME = "__sanitize__";
static const char* COVERAGE_FUNCTION_NAME = "__coverage__";

void instrumentCoverage(Module& M, Instruction& I, int Line, int Col) {
  auto& Context = M.getContext();
  Type* Int32Type = Type::getInt32Ty(Context);

  auto* LineVal = llvm::ConstantInt::get(Int32Type, Line);
  auto* ColVal = llvm::ConstantInt::get(Int32Type, Col);
  std::vector<Value*> Args = {LineVal, ColVal};

  auto* Fun = M.getFunction(COVERAGE_FUNCTION_NAME);
  CallInst::Create(Fun, Args, "", &I);
}

void instrumentSanitize(Module& M, Instruction& I, int Line, int Col) {
  LLVMContext& Context = M.getContext();
  Type* Int32Type = Type::getInt32Ty(Context);

  auto* Divisor = I.getOperand(1);
  auto* DivisorType = cast<IntegerType>(Divisor->getType());
  auto* Zero = ConstantInt::get(DivisorType, 0);
  IRBuilder<> Builder(&I);
  auto* DivisorIsNonZero = Builder.CreateICmpNE(Divisor, Zero, "divisor.nonzero");
  auto* SanitizeValue =
      Builder.CreateZExt(DivisorIsNonZero, Int32Type, "divisor.nonzero.i32");
  auto* LineVal = llvm::ConstantInt::get(Int32Type, Line);
  auto* ColVal = llvm::ConstantInt::get(Int32Type, Col);
  std::vector<Value*> Args = {SanitizeValue, LineVal, ColVal};

  auto* Fun = M.getFunction(SANITIZE_FUNCTION_NAME);
  CallInst::Create(Fun, Args, "", &I);
}

PreservedAnalyses InstrumentDivision::run(Module& M, ModuleAnalysisManager& AM) {
  outs() << "Running " << PASS_DESC << " on module " << M.getName() << "\n";

  auto& Context = M.getContext();
  auto* VoidType = Type::getVoidTy(Context);
  auto* Int32Type = Type::getInt32Ty(Context);

  // Declare external functions
  M.getOrInsertFunction(COVERAGE_FUNCTION_NAME, VoidType, Int32Type, Int32Type);
  M.getOrInsertFunction(
      SANITIZE_FUNCTION_NAME, VoidType, Int32Type, Int32Type, Int32Type);

  for (auto& F : M) {
    if (F.isDeclaration()) {
      continue;
    }

    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
      if (I->getOpcode() == Instruction::PHI) {
        continue;
      }

      const auto DebugLoc = I->getDebugLoc();
      if (!DebugLoc) {
        continue;
      }

      int Line = DebugLoc.getLine();
      int Col = DebugLoc.getCol();
      if (I->getOpcode() == Instruction::SDiv || I->getOpcode() == Instruction::UDiv) {
        instrumentSanitize(M, *I, Line, Col);
      }
      instrumentCoverage(M, *I, Line, Col);
    }
  }

  return PreservedAnalyses::none();
}

}  // namespace instrument
