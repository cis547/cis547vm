#include "BuildDictionary.h"
#include "InstrumentDivision.h"
#include <llvm/Passes/PassPlugin.h>

namespace instrument {

// Pass registration for the new pass manager
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "FuzzingAnalysis", "1.0.0", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                    ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "FuzzingAnalysis") {
                    MPM.addPass(BuildDictionaryPass());
                    MPM.addPass(InstrumentDivision());
                    return true;
                  }
                  return false;
                });
          }};
}

}  // namespace instrument
