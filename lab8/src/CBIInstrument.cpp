#include "CBIInstrument.h"

using namespace llvm;

namespace instrument {

static const char *CBIBranchFunctionName = "__cbi_branch__";
static const char *CBIReturnFunctionName = "__cbi_return__";

/*
 * Implement instrumentation for the branch scheme of CBI.
 */
void instrumentCBIBranches(Module *M, Function &F, BranchInst &I) {
  /* Add your code here */
}

/*
 * Implement instrumentation for the return scheme of CBI.
 */
void instrumentCBIReturns(Module *M, Function &F, CallInst &I) {
  /* Add your code here */
}

bool CBIInstrument::runOnFunction(Function &F) {
  /* Add your code here */
  return true;
}

char CBIInstrument::ID = 1;
static RegisterPass<CBIInstrument> X("CBIInstrument",
                                     "Instrumentations for CBI", false, false);

} // namespace instrument
