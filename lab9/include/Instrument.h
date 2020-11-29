#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace instrument {

static const char *DSEInitFunctionName = "__DSE_Init__";
static const char *DSEAllocaFunctionName = "__DSE_Alloca__";
static const char *DSEStoreFunctionName = "__DSE_Store__";
static const char *DSELoadFunctionName = "__DSE_Load__";
static const char *DSEConstFunctionName = "__DSE_Const__";
static const char *DSERegisterFunctionName = "__DSE_Register__";
static const char *DSEICmpFunctionName = "__DSE_ICmp__";
static const char *DSEBranchFunctionName = "__DSE_Branch__";
static const char *DSEBinOpFunctionName = "__DSE_BinOp__";

int RegisterID = 0;
std::map<Value *, int> RegisterMap;
int BranchID = 0;
std::map<Instruction *, int> BranchMap;

int getRegisterID(Value *I) {
  if (RegisterMap.find(I) == RegisterMap.end()) {
    RegisterMap[I] = RegisterID;
    return RegisterID++;
  } else {
    return RegisterMap[I];
  }
}

int getBranchID(Instruction *I) {
  if (BranchMap.find(I) == BranchMap.end()) {
    BranchMap[I] = BranchID;
    return BranchID++;
  } else {
    return BranchMap[I];
  }
}

struct Instrument : public FunctionPass {
  static char ID;
  static const char *checkFunctionName;

  Instrument() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;
};
} // namespace instrument
