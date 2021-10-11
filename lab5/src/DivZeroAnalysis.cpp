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

/* Some utility functions */

const char *WhiteSpaces = " \t\n\r";

std::string variable(Value *V) {
  std::string Code;
  raw_string_ostream SS(Code);
  V->print(SS);
  Code.erase(0, Code.find_first_not_of(WhiteSpaces));
  return Code;
}

std::string address(Value *V) {
  std::string Code;
  raw_string_ostream SS(Code);
  V->print(SS);
  Code.erase(0, Code.find_first_not_of(WhiteSpaces));
  Code = "@(" + Code + ")";
  return Code;
}

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

/* This function can used to evaluate Instruction::PHI */
Domain *evalPhiNode(PHINode *PHI, const Memory *Mem) {
  /* Add your code from Lab 4 here */
}


// This function is intended to return the union of two Memory objects (M1 and M2), accounting for Domain values
Memory* join(Memory *M1, Memory *M2) {
  /* Add your code from Lab 4 here */
}


/* Return true if the two memories M1 and M2 are equal */
bool equal(Memory *M1, Memory *M2) {
  /* Add your code from Lab 4 here */
}


/* Flow abstract domain from all predecessors of I into the In Memory object for I. */
void DivZeroAnalysis::flowIn(Instruction *I, Memory *In) {
  /* Add your code from Lab 4 here */
}


// Create a transfer function that updates the Out Memory based on In Memory and the instruction type/parameters */
void DivZeroAnalysis::transfer(Instruction *I, const Memory *In, Memory *NOut,
                               PointerAnalysis *PA,
                               SetVector<Value *> PointerSet) {
  if (AllocaInst *AI = dyn_cast<AllocaInst>(I)) {
    // do nothing, this is taken care of in PointerAnalysis.cpp
    return;
  } else if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
    // If the value being stored is not an integer, then don't do anything
    if(!SI->getValueOperand()->getType()->isIntegerTy())
      return;

    /**
     * TODO
     * 1. Evaluate the domain D for SI
     * 2. Get all the pointers that "may-alias" the SI from PA and PointerSet
     * 3. Get the domain of each may-alias of SI from In, and join all those domains as well as domain D
     * 4. Update the domain for SI and each may-aliasing pointer to the new joined domain in NOut
     */
  } else if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
    // if the value being loaded is not of an integer type, then do nothing
    if(!LI->getType()->isIntegerTy())
      return;

    /**
     * TODO
     * 1. Get the pointer operand P of the load instruction (there is a specific getPointerOperand function, check the documentation for details)
     * 2. If P is in In, then update the domain of LI in NOut to the domain of P
     * 3. Otherwise, update the domain of LI in NOut to MaybeZero
     */
  } else if (ReturnInst *RI = dyn_cast<ReturnInst>(I)) {
    // do nothing
    return;
  } else if (BranchInst *BI = dyn_cast<BranchInst>(I)) {
    // do nothing
    return;
  } else {
    /* Add your code from Lab 4 here */
  }
}

/**
 * For a given instruction,
 * check if the pre-transfer memory and post-transfer memory are equal,
 * and update WorkSet as needed.
 **/
void DivZeroAnalysis::flowOut(Instruction *I, Memory *Pre, Memory *Post, SetVector <Instruction *> &WorkSet) {
  /* Add your code from Lab 4 here */
}


void DivZeroAnalysis::doAnalysis(Function &F, PointerAnalysis *PA) {
  // ArgMemory is the In memory for the first instruction
  auto* ArgMemory = new Memory();
  for(auto& Arg : F.args()) {
    (*ArgMemory)[variable(&Arg)] = new Domain(Domain::MaybeZero);
  }

  Instruction* BeginInst = &(*inst_begin(F));
  InMap[BeginInst] = ArgMemory;

  SetVector<Instruction *> WorkSet;
  SetVector<Value *> PointerSet;
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    WorkSet.insert(&(*I));
    PointerSet.insert(&(*I));
  }
  /**
   * Add your code from Lab 4 here.
   * You will have to modify it a bit to use the PointerAnalysis
   */
}


// Check if a specific instruction can incur a divide-by-zero error
bool DivZeroAnalysis::check(Instruction *I) {
  /* Add your code from Lab 4 here */
  return false;
}

void DivZeroAnalysis::collectErrorInsts(Function &F) {
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    if (check(&*I))
      ErrorInsts.insert(&*I);
  }
}

bool DivZeroAnalysis::runOnFunction(Function &F) {
  outs() << "Running " << getAnalysisName() << " on " << F.getName() << "\n";
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    InMap[&(*I)] = new Memory;
    OutMap[&(*I)] = new Memory;
  }

  PointerAnalysis *PA = new PointerAnalysis(F);
  doAnalysis(F, PA);

  collectErrorInsts(F);
  outs() << "Potential Instructions by " << getAnalysisName() << ": \n";
  for (auto I : ErrorInsts) {
    outs() << *I << "\n";
  }

  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    delete InMap[&(*I)];
    delete OutMap[&(*I)];
  }
  return false;
}

char DivZeroAnalysis::ID = 1;
static RegisterPass<DivZeroAnalysis> X("DivZero", "Divide-by-zero Analysis",
                                       false, false);
} // namespace dataflow
