#include "DivZeroAnalysis.h"

namespace dataflow {

//===----------------------------------------------------------------------===//
// DivZero Analysis Implementation
//===----------------------------------------------------------------------===//

/**
 * Implement your data-flow analysis. Feel free to add your own methods as you
 * see fit - it is not compulsory to use the following functions
 * 
 * PART 1
 * 1. Define "transfer" that computes the semantics of each instruction. Also complete the
 *    implementation of evalPhiNode.
 * 2. Define "check" that checks if a given instruction is erroneous or not.
 *
 * PART 2
 * 1. Uncomment and implement "doAnalysis" that stores your results in "InMap" and "OutMap".
 * 2. Uncomment and implement "flowIn" that joins the memory set of all incoming flows
 * 3. Uncomment and implement "flowOut" that flows the memory set to all outgoing flows
 * 4. Uncomment and implement "join" to union two Memory objects, accounting for Domain value
 * 5. Uncomment and implement "equal" to compare two Memory objects
 */


/* PART 1: This function can used to evaluate Instruction::PHI */
Domain *evalPhiNode(PHINode *PHI, const Memory *Mem) {
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


/* PART 2: This function is intended to return the union of two Memory objects (M1 and M2), accounting for Domain values
           Uncomment and implement the join function when you start with PART 2
 */
/*
Memory* join(Memory *M1, Memory *M2) {
  Memory* Result = new Memory();
  // Add your code here
  return Result;
}
*/


/* PART 2: Return true if the two memories M1 and M2 are equal
           Uncomment and implement the equal function when you start with PART 2
 */
/*
bool equal(Memory *M1, Memory *M2) {
  // Add your code here
}
*/


/* PART 2: Flow abstract domain from all predecessors of I into the In Memory object for I.
           Uncomment and implement the flowIn function when you start with PART 2
 */
/*
void DivZeroAnalysis::flowIn(Instruction *I, Memory *In) {
  // Add your code here
}
*/


/* PART 1: Create a transfer function that updates the Out Memory based on In Memory and the instruction type/parameters */
void DivZeroAnalysis::transfer(Instruction *I, const Memory *In, Memory *NOut) {
  // Add your code here
}


/* PART 2: For a given instruction, merge abstract domain from pre-transfer memory to post-transfer memory, and update WorkSet as needed
           Uncomment and implement the flowOut function when you start with PART 2
 */
/*
void DivZeroAnalysis::flowOut(Instruction *I, Memory *Pre, Memory *Post, SetVector <Instruction *> &WorkSet) {
  // Add your code here
}
*/

/* 
PART 2: Uncomment this function when starting part 2 in order to overwrite with your own
        reaching definitions analysis implementation. 
Basic Workflow:
  - Visit instruction in WorkSet
  - For each visited instruction I, construct its In memory by joining all memory sets of incoming flows (predecessors of I)
  - Based on the type of instruction I and the In memory, populate the NOut memory
  - Based on the previous Out memory and the current Out memory, check if there is a difference between the two and
     flow the memory set appropriately to all successors of I and update WorkSet accordingly
*/
/*
void DivZeroAnalysis::doAnalysis(Function &F) { 
  SetVector<Instruction *> WorkSet;
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    WorkSet.insert(&(*I));
  }
  //Add your code here
}
*/


/* PART 1: check if a specific instruction can incur a divide-by-zero error */
bool DivZeroAnalysis::check(Instruction *I) {
  // Add your code here
  return false;
}


char DivZeroAnalysis::ID = 1;
static RegisterPass<DivZeroAnalysis> X("DivZero", "Divide-by-zero Analysis",
                                       false, false);
} // namespace dataflow
