#include "BuildDictionary.h"
#include "InstrumentComparisons.h"
#include "InstrumentDivision.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

namespace instrument {

// Pass registration for the new pass manager
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "FuzzingAnalysis", "1.0.0", [](PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                    ModulePassManager& MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "BuildDictionary") {
                    MPM.addPass(BuildDictionaryPass());
                    return true;
                  }
                  if (Name == "ComparisonLogging") {
                    MPM.addPass(InstrumentComparisons());
                    return true;
                  }
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
