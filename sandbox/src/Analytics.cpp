#include "Analytics.h"

using namespace llvm;

namespace analytics {
Analytics::Analytics() : ModulePass(ID) {}

bool Analytics::runOnModule(Module &M) {
  int NumOfFunctions = 0;
  int NumOfInstructions = 0;

  int NumOfBranchInstrs = 0;
  int NumOfBinaryInstrs = 0;
  int NumOfStoreLoadInstrs = 0;

  Function* Fp = M.getFunction(StringRef("NoName"));
  outs() << "Your function: " << *Fp << "\n";
  /* Goes through all the functions and instructions,
   * counts the functions and instructions while iterating
   * and counts the kinds of instructions by their type.
   */
  for(Module::iterator func = M.begin(); func != M.end(); ++func){
    NumOfFunctions += 1;
    for(Function::iterator FI = func->begin(); FI != func->end(); ++FI){
      for(BasicBlock::iterator instr = FI->begin(); instr !=FI->end(); ++instr){
        NumOfInstructions += 1;
        if(instr->isBinaryOp()){
          NumOfBinaryInstrs += 1;
        } else if (instr->getOpcode() == Instruction::Br){
          NumOfBranchInstrs += 1;
        } else if (instr->getOpcode() == Instruction::Store || instr->getOpcode() == Instruction::Load){
          NumOfStoreLoadInstrs += 1;
        }
      }
    }
  }
  
  outs() << "Analytics of Module " << M.getName() << "\n";
  outs() << "  # Functions    : " << NumOfFunctions << "\n";
  outs() << "  # Instructions : " << NumOfInstructions << "\n";
  outs() << "Instruction count of each kind:\n";
  outs() << "  # Binary       : " << NumOfBinaryInstrs << "\n";
  outs() << "  # Branch       : " << NumOfBranchInstrs << "\n";
  outs() << "  # Store/Load   : " << NumOfStoreLoadInstrs << "\n";
   
  return false;
}

char Analytics::ID = 1;
static RegisterPass<Analytics> X("Analytics", "Analytics", false, false);
} // namespace analytics