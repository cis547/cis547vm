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
 * 1. Define "transfer" that computes the semantics of each instruction.
 * 2. Define "check" that checks if a given instruction is erroneous or not.
 *
 * PART 2
 * 1. Uncomment and implement "doAnalysis" that stores your results in "InMap" and "OutMap".
 * 2. Define "flowIn" that joins the memory set of all incoming flows
 * 3. Define "flowOut" that flows the memory set to all outgoing flows
 * 4. Define "join" to union two Memory objects
 * 5. Define "equal" to compare two Memory objects
 */


//<!---- START DEBUG FUNCTIONS ---->
//
// These debugging functions are provided for your convenience, but feel free 
// to modify these however you like. These will not be graded.
//
void printMem(Memory *Mem) {
  errs() << "[\n";
  for (Memory::iterator V = Mem->begin(), VE = Mem->end(); V != VE; ++V) {
    errs() << "  " << V->first << " => " << *V->second << ";\n";
  }
  errs() << "]\n";
}

void printMap(Function &F, ValueMap<Instruction *, Memory *> &InMap,
              ValueMap<Instruction *, Memory *> &OutMap) {
  errs() << "Dataflow Analysis Results:\n";
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    errs() << "Instruction: " << *I << "\n";
    errs() << "In set: \n";
    Memory *InMem = InMap[&(*I)];
    printMem(InMem);
    errs() << "Out set: \n";
    Memory *OutMem = OutMap[&(*I)];
    printMem(OutMem);
    errs() << "\n";
  }
}
//<!---- END DEBUG FUNCTIONS ---->


/* PART 2: This function can used to evaluate Instruction::PHI */
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


/* PART 2: Return the union of two Memory objects (M1 and M2), accounting for Domain values */
Memory* join(Memory *M1, Memory *M2) {
  Memory* Result = new Memory();
  // Add your code here
  return Result;
}


/* PART 2: Return true if M1 is less than or equal to M2, i.e., M2 contains everything in M1 */
bool equal(Memory *M1, Memory *M2) {
  // Add your code here
}


/* PART 2: Flow abstract domain from all predecessors of I into the In Memory object for I */
void DivZeroAnalysis::flowIn(Instruction *I, Memory *In) {
  // Add your code here
}


/* PART 1: Create a transfer function that updates the Out Memory based on In Memory and the instruction type/parameters */
void DivZeroAnalysis::transfer(Instruction *I, const Memory *In, Memory *NOut) {
  // Add your code here
}


/* PART 2: For a given instruction, merge abstract domain from pre-transfer memory to post-transfer memory, and update WorkSet as needed */
void DivZeroAnalysis::flowOut(Instruction *I, Memory *Pre, Memory *Post, SetVector <Instruction *> &WorkSet) {
  // Add your code here
}


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
