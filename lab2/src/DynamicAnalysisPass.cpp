#include "Instrument.h"
#include "Utils.h"

using namespace llvm;

namespace instrument {

const auto PASS_DESC = "Dynamic Analysis Pass";
const auto COVERAGE_FUNCTION_NAME = "__coverage__";
const auto BINOP_OPERANDS_FUNCTION_NAME = "__binop_op__";

void instrumentCoverage(Module *M, Instruction &I, int Line, int Col);
void instrumentBinOpOperands(Module *M, BinaryOperator *BinOp, int Line, int Col);

PreservedAnalyses DynamicAnalysisPass::run(Module &M, ModuleAnalysisManager &AM) {
  errs() << "Running " << PASS_DESC << " on module " << M.getName() << "\n";

  LLVMContext &Context = M.getContext();
  Type *VoidType = Type::getVoidTy(Context);
  Type *Int32Type = Type::getInt32Ty(Context);
  Type *Int8Type = Type::getInt8Ty(Context);

  // Declare external functions
  M.getOrInsertFunction(COVERAGE_FUNCTION_NAME, VoidType, Int32Type, Int32Type);
  M.getOrInsertFunction(BINOP_OPERANDS_FUNCTION_NAME,
      VoidType,
      Int8Type,
      Int32Type,
      Int32Type,
      Int32Type,
      Int32Type);

  for (auto &F : M) {
    if (F.isDeclaration())
      continue;

    auto FunctionName = F.getName().str();
    errs() << "Instrumenting function " << FunctionName << "\n";

    errs() << "Instrument Instructions\n";

    // Store instructions to instrument to avoid iterator invalidation
    std::vector<std::pair<Instruction *, std::pair<int, int>>> toInstrument;

    for (inst_iterator Iter = inst_begin(F), E = inst_end(F); Iter != E; ++Iter) {
      Instruction &Inst = (*Iter);
      llvm::DebugLoc DebugLoc = Inst.getDebugLoc();
      if (!DebugLoc) {
        // Skip Instruction if it doesn't have debug information.
        continue;
      }

      int Line = DebugLoc.getLine();
      int Col = DebugLoc.getCol();
      toInstrument.push_back({&Inst, {Line, Col}});
    }

    // Now instrument the collected instructions
    for (auto &instInfo : toInstrument) {
      Instruction *Inst = instInfo.first;
      int Line = instInfo.second.first;
      int Col = instInfo.second.second;

      instrumentCoverage(&M, *Inst, Line, Col);

      /**
       * TODO: Add code to check if the instruction is a BinaryOperator and if so,
       * instrument the instruction as specified in the Lab document.
       */
    }
  }

  return PreservedAnalyses::none();
}

void instrumentCoverage(Module *M, Instruction &I, int Line, int Col) {
  auto &Context = M->getContext();
  auto *Int32Type = Type::getInt32Ty(Context);

  auto LineVal = ConstantInt::get(Int32Type, Line);
  auto ColVal = ConstantInt::get(Int32Type, Col);

  std::vector<Value *> Args = {LineVal, ColVal};

  auto *CoverageFunction = M->getFunction(COVERAGE_FUNCTION_NAME);
  CallInst::Create(CoverageFunction, Args, "", &I);
}

void instrumentBinOpOperands(Module *M, BinaryOperator *BinOp, int Line, int Col) {
  auto &Context = M->getContext();
  auto *Int32Type = Type::getInt32Ty(Context);
  auto *CharType = Type::getInt8Ty(Context);

  /**
   * TODO: Add code to instrument the BinaryOperator to print
   * its location, operation type and the runtime values of its
   * operands.
   */
}

// Pass registration for the new pass manager
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "DynamicAnalysisPass", "1.0.0", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                    ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "DynamicAnalysisPass") {
                    MPM.addPass(DynamicAnalysisPass());
                    return true;
                  }
                  return false;
                });
          }};
}

}  // namespace instrument
