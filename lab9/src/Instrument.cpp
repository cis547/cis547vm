#include "Instrument.h"

using namespace llvm;

namespace instrument {

void instrumentDSEInit(Module *M, Function &F, Instruction &I) {
  // fill your instrumentation code here
}

void instrumentDSEAlloca(Module *M, AllocaInst *AI) {
  LLVMContext &Ctx = M->getContext();
  std::vector<Value *> Args;
  Value *V = ConstantInt::get(Type::getInt32Ty(Ctx), getRegisterID(AI));
  Args.push_back(V);
  Args.push_back(AI);
  Type *ArgsTypes[] = {Type::getInt32Ty(Ctx), Type::getInt32PtrTy(Ctx)};
  FunctionType *FType = FunctionType::get(Type::getVoidTy(Ctx), ArgsTypes, false);
  Value *Fun = M->getOrInsertFunction(DSEAllocaFunctionName, FType);
  if (Function *F = dyn_cast<Function>(Fun)) {
      CallInst *Call = CallInst::Create(Fun, Args, "", AI->getNextNonDebugInstruction());
      Call->setCallingConv(CallingConv::C);
      Call->setTailCall(true);
  } else {
      errs() << "WARN: invalid function\n";
  }
}

void instrumentDSEStore(Module *M, StoreInst *SI) {
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

void instrumentDSELoad(Module *M, LoadInst *LI) {
  // fill your instrumentation code here
}

void instrumentDSEConst(Module *M, Value *V, Instruction *I) {
  // fill your instrumentation code here
}

void instrumentDSERegister(Module *M, Value *V, Instruction *I) {
  // fill your instrumentation code here
}

void instrumentDSEICmp(Module *M, ICmpInst *I) {
  // fill your instrumentation code here
}

void instrumentDSEBranch(Module *M, BranchInst *BI) {
  // fill your instrumentation code here
}

void instrumentDSEBinOp(Module *M, BinaryOperator *BO) {
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
