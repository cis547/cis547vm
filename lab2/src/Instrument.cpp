#include "Instrument.h"

using namespace llvm;

namespace instrument {

static const char *SanitizerFunctionName = "__sanitize__";
static const char *CoverageFunctionName = "__coverage__";

/*
 * Implement divide-by-zero sanitizer.
 */
void instrumentSanitize(Module *M, Instruction &I) {
  /* Add you code here */
}

/*
 * Implement code coverage instrumentation.
 */
void instrumentCoverage(Module *M, Instruction &I) {
  /* Add you code here */
}

bool Instrument::runOnFunction(Function &F) {
  /* Add you code here */
  return true;
}

char Instrument::ID = 1;
static RegisterPass<Instrument>
    X("Instrument", "Instrumentations for Dynamic Analysis", false, false);

} // namespace instrument
