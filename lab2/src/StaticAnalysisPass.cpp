#include "Instrument.h"
#include "Utils.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace instrument {

const auto PASS_DESC = "Static Analysis Pass";

PreservedAnalyses StaticAnalysisPass::run(Module &M, ModuleAnalysisManager &AM) {
  outs() << "Running " << PASS_DESC << " on module " << M.getName() << "\n";

  for (auto &F : M) {
    if (F.isDeclaration())
      continue;

    auto FunctionName = F.getName().str();
    outs() << "Running " << PASS_DESC << " on function " << FunctionName << "\n";

    outs() << "Locating Instructions\n";
    for (inst_iterator Iter = inst_begin(F), E = inst_end(F); Iter != E; ++Iter) {
      Instruction &Inst = (*Iter);
      llvm::DebugLoc DebugLoc = Inst.getDebugLoc();
      if (!DebugLoc) {
        // Skip Instruction if it doesn't have debug information.
        continue;
      }

      int Line = DebugLoc.getLine();
      int Col = DebugLoc.getCol();
      outs() << Line << ", " << Col << "\n";

      /**
       * TODO: Add code to check if the instruction is a BinaryOperator and if
       * so, print the information about its location and operands as specified
       * in the Lab document.
       */
    }
  }
  return PreservedAnalyses::all();
}

// Pass registration for the new pass manager
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "StaticAnalysisPass", "1.0.0", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                    ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "StaticAnalysisPass") {
                    MPM.addPass(StaticAnalysisPass());
                    return true;
                  }
                  return false;
                });
          }};
}

}  // namespace instrument
