#include "DivZeroAnalysis.h"

namespace dataflow {

//===----------------------------------------------------------------------===//
// DivZero Analysis Implementation
//===----------------------------------------------------------------------===//

/**
 * Implement your data-flow analysis with pointer-aliasing functionality. 
 * Feel free to add your own methods as you see fit - it is not compulsory to use the provided functions
 * 
 * You should copy your implementation of the previous DivZeroAnalysis code
 * NOTE: Do not overwrite the method signatures for transfer() and doAnalysis() - these have
 *       been updated to include classes that will faciliate pointer aliasing!
 * 
 * 
 * 
 * PART 1: Handle function arguments and any CallInst iff it returns an int.
 *
 * PART 2: Handle StoreInst and LoadInst, accounting for may-aliases when pointers are present
 */


// This function can used to evaluate Instruction::PHI */
Domain *evalPhiNode(PHINode *PHI, const Memory *Mem) {
  /* Add your code here */
  Value* cv = PHI->hasConstantValue();
  if(cv){
    // Add your code here
    // eval cv, manipulate Mem, return
  }
  unsigned int n = PHI->getNumIncomingValues();
  Domain* joined = NULL;
  for(unsigned int i = 0; i < n; i++){
    // Add your code here
    // eval PHI->getIncomingValue(i), translate it to a Domain, manipulate Mem
    Domain* V = NULL;
    if(!joined){
      joined = V;
    }
    joined = Domain::join(joined, V);
  }
  return joined;
}


// This function is intended to return the union of two Memory objects (M1 and M2), accounting for Domain values
Memory* join(Memory *M1, Memory *M2) {
  /* Add your code here */
  Memory* Result = new Memory();
  return Result;
}


// Return true if the two memories M1 and M2 are strictly equal
bool equal(Memory *M1, Memory *M2) {
  //* Add your code here */
  return true;
}


// Flow abstract domain from all predecessors of I into the In Memory object for I.
void DivZeroAnalysis::flowIn(Instruction *I, Memory *In) {
  /* Add your code here */
}


// Create a transfer function that updates the Out Memory based on In Memory and the instruction type/parameters */
void DivZeroAnalysis::transfer(Instruction *I, const Memory *In, Memory *NOut,
                               PointerAnalysis *PA,
                               SetVector<Value *> PointerSet) {
  /* Add your code here */
}


// For a given instruction, check if the pre-transfer memory and post-transfer memory are equal, and update WorkSet as needed
void DivZeroAnalysis::flowOut(Instruction *I, Memory *Pre, Memory *Post, SetVector <Instruction *> &WorkSet) {
  /* Add your code here */
}


void DivZeroAnalysis::doAnalysis(Function &F, PointerAnalysis *PA) {
  SetVector<Instruction *> WorkSet;
  SetVector<Value *> PointerSet;
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    WorkSet.insert(&(*I));
    PointerSet.insert(&(*I));
  }

  /* Add your code here */
  /* Basic Workflow-
       Visit instruction in WorkSet
       For each visited instruction I, construct its In memory by joining all memory sets of incoming flows (predecessors of I)
       Based on the type of instruction I and the In memory, populate the NOut memory. Take the pointer analysis into consideration for this step
       Based on the previous Out memory and the current Out memory, check if there is a difference between the two and
         flow the memory set appropriately to all successors of I and update WorkSet accordingly
  */ 
}


// Check if a specific instruction can incur a divide-by-zero error
bool DivZeroAnalysis::check(Instruction *I) {
  /* Add your code here */
  return false;
}


char DivZeroAnalysis::ID = 1;
static RegisterPass<DivZeroAnalysis> X("DivZero", "Divide-by-zero Analysis",
                                       false, false);
} // namespace dataflow
