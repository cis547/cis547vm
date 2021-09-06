#include "Instrument.h"

using namespace llvm;

namespace instrument {

static const char *SanitizeFunctionName = "__sanitize__";
static const char *CoverageFunctionName = "__coverage__";

/**
 * Implement divide-by-zero sanitizer.
 * Handle different Instructions appropriately (Ex. different operations etc.)
 * Retrieve line and column values from Instruction DebugLoc
 * Insert the __sanitize__ function from runtime.c into llvm with the required arg types
 * Call the __sanitize__ function with the required arguments parsed from Instruction and DebugLoc
 */
void instrumentSanitize(Module *M, Instruction &I) {
  /* Add you code here */
}

/**
 * Implement code coverage instrumentation.
 * Retrieve line and column values from Instruction DebugLoc
 * Insert the __coverage__ function from runtime.c into llvm with the required arg types
 * Call the __coverage__ function with the required arguments parsed from DebugLoc
 */
void instrumentCoverage(Module *M, Instruction &I) {
  /* Add you code here */
}

/**
 * Implement run instrumentation.
 * Iterate through a given Function's Instructions and pass the required arguments to instrumentSanitize() and instrumentCoverage()
 * Every instruction with a DebugLoc must be instrumented with the __coverage__ function
 * Only instructions with division operators must be instrumented with the __sanitize__ function
 */
bool Instrument::runOnFunction(Function &F) {
  /* Add you code here */
  return true;
}

char Instrument::ID = 1;
static RegisterPass<Instrument>
    X("Instrument", "Instrumentations for Dynamic Analysis", false, false);

} // namespace instrument