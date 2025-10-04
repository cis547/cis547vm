#include "InstrumentDivision.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace instrument {

static const auto PASS_NAME = "Instrument";
static const auto PASS_DESC = "Instrumentation for Division";
static const char *SANITIZE_FUNCTION_NAME = "__sanitize__";
static const char *COVERAGE_FUNCTION_NAME = "__coverage__";

void instrumentCoverage(Module &M, Instruction &I, int Line, int Col) {
  auto &Context = M.getContext();
  Type *Int32Type = Type::getInt32Ty(Context);

  auto *LineVal = llvm::ConstantInt::get(Int32Type, Line);
  auto *ColVal = llvm::ConstantInt::get(Int32Type, Col);
  std::vector<Value *> Args = {LineVal, ColVal};

  auto *Fun = M.getFunction(COVERAGE_FUNCTION_NAME);
  CallInst::Create(Fun, Args, "", &I);
}

void instrumentSanitize(Module &M, Instruction &I, int Line, int Col) {
  LLVMContext &Context = M.getContext();
  Type *Int32Type = Type::getInt32Ty(Context);

  auto *Divisor = I.getOperand(1);
  auto *LineVal = llvm::ConstantInt::get(Int32Type, Line);
  auto *ColVal = llvm::ConstantInt::get(Int32Type, Col);
  std::vector<Value *> Args = {Divisor, LineVal, ColVal};

  auto *Fun = M.getFunction(SANITIZE_FUNCTION_NAME);
  CallInst::Create(Fun, Args, "", &I);
}

PreservedAnalyses InstrumentDivision::run(Module &M, ModuleAnalysisManager &AM) {
  outs() << "Running " << PASS_DESC << " on module " << M.getName() << "\n";

  auto &Context = M.getContext();
  auto *VoidType = Type::getVoidTy(Context);
  auto *Int32Type = Type::getInt32Ty(Context);

  // Declare external functions
  M.getOrInsertFunction(COVERAGE_FUNCTION_NAME, VoidType, Int32Type, Int32Type);
  M.getOrInsertFunction(
      SANITIZE_FUNCTION_NAME, VoidType, Int32Type, Int32Type, Int32Type);

  for (auto &F : M) {
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

// Pass registration for the new pass manager
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, "1.0.0", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                    ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == PASS_NAME) {
                    MPM.addPass(InstrumentDivision());
                    return true;
                  }
                  return false;
                });
          }};
}

}  // namespace instrument
