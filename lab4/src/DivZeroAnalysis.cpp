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

/* Some utility functions */

bool isConstantData(Value *V) { return isa<ConstantData>(V); }

bool isZero(Value *V) {
  if (ConstantData *CD = dyn_cast<ConstantData>(V))
    return CD->isZeroValue();
  else
    return false;
}

bool contain(const std::string &Key, const Memory *Mem) {
  if (Mem->find(Key) == Mem->end())
    return false;
  return true;
}

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

/* PART 1: This function can used to evaluate Instruction::PHI */
Domain *evalPhiNode(PHINode *PHI, const Memory *Mem) {
  Value* cv = PHI->hasConstantValue();
  if(cv){
    // evaluate the domain for cv
    // return the domain for cv
    
    // Add your code here
  }
  unsigned int n = PHI->getNumIncomingValues();
  Domain* joined = NULL;
  for(unsigned int i = 0; i < n; i++){
    // Add your code here
    auto incoming = PHI->getIncomingValue(i);
    Domain* V = NULL;
    // evaluate the domain of `incoming'
    // assign the pointer to the domain of `incoming' to V
    if(!joined){
      joined = V;
    }
    joined = Domain::join(joined, V);
  }
  return joined;
}


/* PART 2: This function is intended to return the union of two Memory objects (M1 and M2), accounting for Domain values
           Uncomment and implement the join function when you start with PART 2
    Basic Workflow:
    - If some instruction I with domain D is either in M1 or M2, but not both, add instruction I with domain D to the Result
    - If some instruction I is present in M1 with domain D1 and in M2 with domain D2, then use Domain::join to find the
      new domain D from D1 and D2, and add instruction I with domain D to the Result
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
    
    Basic Workflow:
    - If any instruction I is present in one of M1 or M2, but not both, the memories are unequal
    - If any instruction I is present in M1 with domain D1 and in M2 with domain D2, if D1 and D2 are unequal, then the memories are unequal
 */
/*
bool equal(Memory *M1, Memory *M2) {
  // Add your code here
}
*/


/* PART 2: Flow abstract domain from all predecessors of I into the In Memory object for I.
           Uncomment and implement the flowIn function when you start with PART 2

    Basic Workflow:
    - For each predecessor P of instruction I, do the following:
      * Get the Out Memory OM for instruction P using OutMap
      * Join memories OM and In
 */
/*
void DivZeroAnalysis::flowIn(Instruction *I, Memory *In) {
  // Add your code here
}
*/


/* PART 1: Create a transfer function that updates the Out Memory based on In Memory and the instruction type/parameters */
void DivZeroAnalysis::transfer(Instruction *I, const Memory *In, Memory *NOut) {
  if (AllocaInst *AI = dyn_cast<AllocaInst>(I)) {
    // do nothing
    return;
  } else if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
    // do nothing
    return;
  } else if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
    // do nothing
    return;
  } else if (ReturnInst *RI = dyn_cast<ReturnInst>(I)) {
    // do nothing
    return;
  } else if (BranchInst *BI = dyn_cast<BranchInst>(I)) {
    // do nothing
    return;
  } else {
    // Add your code here for the rest of the instruction types
    // You must consider the following instructions:
    //   1. BinaryOperator: For +, -, * and /, evaluate the Domain based on the Domains of the operators
    //   2. CastInst
    //   3. CmpInst: If comparison is <, <=, >=, >, then let the new Domain be MaybeZero. Otherwise, for == and !=,
    //        evaluate the Domain based on the Domains of the operators. Feel free to make it more complex if you wish
    //   4. PHINode
    //   5. input instructions (check using the `isInput' function)
  }
}


/* PART 2: For a given instruction, merge abstract domain from pre-transfer OUT memory to post-transfer OUT memory, and update WorkSet as needed
           Uncomment and implement the flowOut function when you start with PART 2

    Basic Workflow:
    - If Pre and Post are unequal, then add all the successors of I in WorkSet and use the Post memory as the OUT memory for I
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
  - For each visited instruction I, construct its In memory by joining all memory sets of incoming flows (predecessors of I) (do this using `flowIn')
  - Based on the type of instruction I and the In memory, populate the NOut memory
  - Based on the previous Out memory and the current Out memory, check if there is a difference between the two and
     flow the memory set appropriately to all successors of I and update WorkSet accordingly (do this using `flowOut')
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
  // Get the opcode for I
  // If it matches the opcode for Signed/Unsigned Division, check the domain
  // Return true if domain is Zero or MaybeZero
  return false;
}


char DivZeroAnalysis::ID = 1;
static RegisterPass<DivZeroAnalysis> X("DivZero", "Divide-by-zero Analysis",
                                       false, false);
} // namespace dataflow
