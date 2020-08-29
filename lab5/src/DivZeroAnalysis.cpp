#include "DivZeroAnalysis.h"

namespace dataflow {

//===----------------------------------------------------------------------===//
// Dataflow Analysis Implementation
//===----------------------------------------------------------------------===//

/**
 * Implement your data-flow analysis.
 * 1. Define "flowIn" that joins the memory set of all incoming flows
 * 2. Define "transfer" that computes the semantics of each instruction.
 * 3. Define "flowOut" that flows the memory set to all outgoing flows
 * 4. Define "doAnalysis" that stores your results in "InMap" and "OutMap".
 * 5. Define "check" that checks if a given instruction is erroneous or not.
 */


// define the following functions if needed (not compulsory to do so)
Memory* join(Memory *M1, Memory *M2) {
  Memory* Result = new Memory();
  /* Add your code here */

  /* Result will be the union of memories M1 and M2 */

  return Result;
}

bool equal(Memory *M1, Memory *M2) {
  /* Add your code here */
  /* Return true if the two memories M1 and M2 are equal */
}


void DivZeroAnalysis::flowIn(Instruction *I, Memory *In) {
  /* Add your code here */
}


void DivZeroAnalysis::transfer(Instruction *I, const Memory *In, Memory *NOut,
                               PointerAnalysis *PA,
                               SetVector<Value *> PointerSet) {
  /* Add your code here */
}

void DivZeroAnalysis::flowOut(Instruction *I, Memory *Pre, Memory *Post,  SetVector <Instruction *> &WorkSet) {
  /* Add your code here */
}

void DivZeroAnalysis::doAnalysis(Function &F, PointerAnalysis *PA) {
  /* Add your code here */
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

bool DivZeroAnalysis::check(Instruction *I) {
  /* Add your code here */
  return false;
}

char DivZeroAnalysis::ID = 1;
static RegisterPass<DivZeroAnalysis> X("DivZero", "Divide-by-zero Analysis",
                                       false, false);
} // namespace dataflow
