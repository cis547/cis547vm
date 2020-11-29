#include "Instrument.h"

using namespace llvm;

namespace instrument {

void instrumentDSEInit(Module *M, Function &F, Instruction &I) {
  // fill your instrumentation code here
}

void instrumentAlloca(Module *M, AllocaInst *AI) {
  // fill your instrumentation code here
}

void instrumentStore(Module *M, StoreInst *SI) {
    LLVMContext &Ctx = M->getContext();
    std::vector<Value *> Args;
    Args.push_back(SI->getPointerOperand());
    Type *ArgsTypes[] = {Type::getInt32PtrTy(Ctx)};
    FunctionType *FType =
        FunctionType::get(Type::getVoidTy(Ctx), ArgsTypes, false);
    Value *Fun = M->getOrInsertFunction(DSEStoreFunctionName, FType);
    if (Function *F = dyn_cast<Function>(Fun)) {
        CallInst *Call = CallInst::Create(Fun, Args, "", SI);
        Call->setCallingConv(CallingConv::C);
        Call->setTailCall(true);
    } else {
        errs() << "WARN: invalid function\n";
    }
}

void instrumentLoad(Module *M, LoadInst *LI) {
  // fill your instrumentation code here
}

void instrumentICmp(Module *M, ICmpInst *I) {
  // fill your instrumentation code here
}

void instrumentBranch(Module *M, BranchInst *BI) {
  // fill your instrumentation code here
}

void instrumentBinOp(Module *M, BinaryOperator *BO) {
  // fill your instrumentation code here
}


/*
 * Implement your instrumentation for dynamic symbolic execution engine
 */
bool Instrument::runOnFunction(Function &F) {
  /* Add your code here */
  return true;
}

char Instrument::ID = 1;
static RegisterPass<Instrument>
    X("Instrument", "Instrumentations for Dynamic Symbolic Execution", false,
      false);

} // namespace instrument
