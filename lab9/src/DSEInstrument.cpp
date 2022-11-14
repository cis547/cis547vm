#include "DSEInstrument.h"

using namespace llvm;

namespace dse {

static const char *DSE_INIT_FUNCTION_NAME = "__DSE_Init__";
static const char *DSE_ALLOCA_FUNCTION_NAME = "__DSE_Alloca__";
static const char *DSE_STORE_FUNCTION_NAME = "__DSE_Store__";
static const char *DSE_LOAD_FUNCTION_NAME = "__DSE_Load__";
static const char *DSE_CONST_FUNCTION_NAME = "__DSE_Const__";
static const char *DSE_REGISTER_FUNCTION_NAME = "__DSE_Register__";
static const char *DSE_ICMP_FUNCTION_NAME = "__DSE_ICmp__";
static const char *DSE_BRANCH_FUNCTION_NAME = "__DSE_Branch__";
static const char *DSE_BINOP_FUNCTION_NAME = "__DSE_BinOp__";


/**
 * @brief Instrument to initialize the Z3 solver.
 *
 * TODO: Implement this.
 *
 * @param Mod Current Module
 * @param F Function to Instrument
 * @param I Instrumentation location
 */
void instrumentDSEInit(Module *Mod, Function &F, Instruction &I) {
}

/**
 * @brief Instrument to Alloca Instructions.
 *
 * @param Mod Current Module
 * @param AI Instrumentation location
 */
void instrumentAlloca(Module *Mod, AllocaInst *AI) {
  auto &Context = Mod->getContext();
  auto *Int32Type = Type::getInt32Ty(Context);

  Value *VarID = ConstantInt::get(Int32Type, getRegisterID(AI));
  std::vector<Value *> Args = {VarID, AI};

  auto Fun = Mod->getFunction(DSE_ALLOCA_FUNCTION_NAME);
  CallInst::Create(Fun, Args, "", AI->getNextNonDebugInstruction());
}

/**
 * @brief Instrument to Store Instructions.
 *
 * @param Mod Current Module
 * @param SI Instrumentation location
 */
void instrumentStore(Module *Mod, StoreInst *SI) {
  std::vector<Value *> Args = {SI->getPointerOperand()};

  auto Fun = Mod->getFunction(DSE_STORE_FUNCTION_NAME);
  CallInst::Create(Fun, Args, "", SI);
}

/**
 * @brief Instrument to Load Instructions.
 *
 * TODO: Implement this.
 *
 * @param Mod Current Module
 * @param LI Instrumentation location
 */
void instrumentLoad(Module *Mod, LoadInst *LI) {
}

/**
 * @brief Instrument Constant values.
 *
 * TODO: Implement this.
 *
 * @param Mod Current Module
 * @param ConstInt Constant
 * @param I Instrumentation location.
 */
void instrumentConstantValue(Module *Mod, ConstantInt *ConstInt, Instruction *I) {
}

/**
 * @brief Instrument Registers.
 *
 * TODO: Implement this.
 *
 * @param Mod Current Module
 * @param Var Variable
 * @param I Instrumentation location
 */
void instrumentRegister(Module *Mod, Value *Var, Instruction *I) {
}

/**
 * @brief Instrument a Value
 *
 * TODO: Implement this.
 *
 * Hint: Values are a stored in registers;
 *       some values may also be a constant.
 *       Use the function you just defined above.
 *
 * @param Mod Current Module
 * @param Val Value
 * @param I Instrumentation location
 */
void instrumentValue(Module *Mod, Value *Val, Instruction *I) {
}

/**
 * @brief Instrument Comparision Instructions.
 *
 * TODO: Implement this.
 *
 * @param Mod Current Module
 * @param CI Instrumentation location
 */
void instrumentICmp(Module *Mod, ICmpInst *CI) {
}

/**
 * @brief Instrument Branch Instructions.
 *
 * TODO: Implement this.
 *
 * @param Mod Current Module
 * @param BI Instrumentation location
 */
void instrumentBranch(Module *Mod, BranchInst *BI) {
}

/**
 * @brief Instrument BinaryOperator.
 *
 * TODO: Implement this.
 *
 * @param Mod Current Module
 * @param BO Instrumentation location
 */
void instrumentBinOp(Module *Mod, BinaryOperator *BO) {
}

/**
 * @brief Instrument Instructions.
 *
 * TODO: Implement this.
 *
 * Hint: Make sure to instrument all the Values
 *       used by an instruction so that they are available
 *       to the DSE Engine.
 *
 * @param Mod Current Module
 * @param I Instruction to Instrument
 */
void instrument(Module *Mod, Instruction *I) {
  if (AllocaInst *AI = dyn_cast<AllocaInst>(I)) {
    // TODO: Implement.
  } else if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
    // TODO: Implement.
  } else if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
    // TODO: Implement.
  } else if (ICmpInst *CI = dyn_cast<ICmpInst>(I)) {
    // TODO: Implement.
  } else if (BranchInst *BI = dyn_cast<BranchInst>(I)) {
    if (BI->isUnconditional())
      return;
    // TODO: Implement.
  } else if (BinaryOperator *BO = dyn_cast<BinaryOperator>(I)) {
    // TODO: Implement.
  }
}

bool DSEInstrument::runOnFunction(Function &F) {
  LLVMContext &Context = F.getContext();
  Module *Mod = F.getParent();

  Type *VoidType = Type::getVoidTy(Context);
  Type *Int1Type = Type::getInt1Ty(Context);
  auto *Int32Type = Type::getInt32Ty(Context);
  Type *Int32PtrType = Type::getInt32PtrTy(Context);

  // Insert all the DSE function declarations into Module.
  Mod->getOrInsertFunction(DSE_INIT_FUNCTION_NAME, VoidType);
  Mod->getOrInsertFunction(DSE_ALLOCA_FUNCTION_NAME, VoidType, Int32Type, Int32PtrType);
  Mod->getOrInsertFunction(DSE_STORE_FUNCTION_NAME, VoidType, Int32PtrType);
  Mod->getOrInsertFunction(DSE_LOAD_FUNCTION_NAME, VoidType, Int32Type, Int32PtrType);
  Mod->getOrInsertFunction(DSE_CONST_FUNCTION_NAME, VoidType, Int32Type);
  Mod->getOrInsertFunction(DSE_REGISTER_FUNCTION_NAME, VoidType, Int32Type);
  Mod->getOrInsertFunction(DSE_ICMP_FUNCTION_NAME, VoidType, Int32Type, Int32Type);
  Mod->getOrInsertFunction(DSE_BRANCH_FUNCTION_NAME, VoidType, Int32Type, Int32Type, Int1Type);
  Mod->getOrInsertFunction(DSE_BINOP_FUNCTION_NAME, VoidType, Int32Type, Int32Type);

  if (F.getName().equals("main")) {
    // TODO: Initilize the DSE Engine
  }

  // Instrument each instruction
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    instrument(Mod, &*I);
  }
  return true;
}

char DSEInstrument::ID = 1;
static RegisterPass<DSEInstrument>
    X("DSEInstrument", "Instrumentations for Dynamic Symbolic Execution", false,
      false);

} // namespace dse
