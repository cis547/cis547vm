#include "CBIInstrument.h"

#include <llvm/Passes/PassPlugin.h>

using namespace llvm;

namespace instrument {

const auto PASS_NAME = "CBIInstrument";
const auto PASS_DESC = "Instrumentation for CBI";
const auto CBI_BRANCH_FUNCTION_NAME = "__cbi_branch__";
const auto CBI_RETURN_FUNCTION_NAME = "__cbi_return__";

/**
 * @brief Instrument a BranchInst with calls to __cbi_branch__
 *
 * @param M Module containing Branch
 * @param Branch A conditional Branch Instruction
 * @param Line Line number of Branch
 * @param Col Coulmn number of Branch
 */
void instrumentBranch(Module *M, BranchInst *Branch, int Line, int Col);

/**
 * @brief Instrument the return value of CallInst using calls to __cbi_return__
 *
 * @param M Module containing Call
 * @param Call A Call instruction that returns an Int32.
 * @param Line Line number of the Call
 * @param Col Column number of the Call
 */
void instrumentReturn(Module *M, CallInst *Call, int Line, int Col);

PreservedAnalyses CBIInstrument::run(Module &M, ModuleAnalysisManager &AM) {
  outs() << "Running " << PASS_DESC << " on module " << M.getName() << "\n";

  auto &Context = M.getContext();
  auto *VoidType = Type::getVoidTy(Context);
  auto *Int32Type = Type::getInt32Ty(Context);
  auto *BoolType = Type::getInt1Ty(Context);

  // Declare external functions
  M.getOrInsertFunction(
      CBI_BRANCH_FUNCTION_NAME, VoidType, Int32Type, Int32Type, BoolType);

  M.getOrInsertFunction(
      CBI_RETURN_FUNCTION_NAME, VoidType, Int32Type, Int32Type, Int32Type);

  for (auto &F : M) {
    if (F.isDeclaration()) {
      continue;
    }

    auto FunctionName = F.getName().str();
    outs() << "Running " << PASS_DESC << " on function " << FunctionName << "\n";

    for (inst_iterator Iter = inst_begin(F), E = inst_end(F); Iter != E; ++Iter) {
      Instruction &Inst = (*Iter);
      llvm::DebugLoc DebugLoc = Inst.getDebugLoc();
      if (!DebugLoc) {
        // Skip Instruction if it doesn't have debug information.
        continue;
      }

      int Line = DebugLoc.getLine();
      int Col = DebugLoc.getCol();

      /**
     * TODO: Add code to check the type of instruction
     * and call appropriate instrumentation function.
     *
     * If Inst is a Branch Instruction then
     *   use instrumentBranch
     * Else if it is a Call returning an int then
     *   use instrumentReturn
     * @param Branch
     */
    }
  }

  return PreservedAnalyses::none();
}

/**
 * Implement instrumentation for the branch scheme of CBI.
 */
void instrumentBranch(Module *M, BranchInst *Branch, int Line, int Col) {
  auto &Context = M->getContext();
  auto Int32Type = Type::getInt32Ty(Context);

  /**
   * TODO: Add code to instrument the Branch Instruction.
   */
}

/**
 * Implement instrumentation for the return scheme of CBI.
 */
void instrumentReturn(Module *M, CallInst *Call, int Line, int Col) {
  auto &Context = M->getContext();
  auto Int32Type = Type::getInt32Ty(Context);

  /**
   * TODO: Add code to instrument the Call Instruction.
   *
   * Note: CallInst::Create(.) follows Insert Before semantics.
   */
}

// Pass registration for the new pass manager
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, "1.0.0", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                    ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == PASS_NAME) {
                    MPM.addPass(CBIInstrument());
                    return true;
                  }
                  return false;
                });
          }};
}

}  // namespace instrument
