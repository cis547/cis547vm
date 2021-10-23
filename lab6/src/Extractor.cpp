#include "Extractor.h"

#include "llvm/IR/Instruction.h"

void Extractor::initialize() {
  /* Relations for Def and Use */
  Solver->register_relation(Def);
  Solver->register_relation(Use);

  /* Relations for Reaching Definition Analysis */
  Solver->register_relation(Kill);
  Solver->register_relation(Next);
  Solver->register_relation(In);
  Solver->register_relation(Out);

  /* Relations for Taint Analysis */
  Solver->register_relation(Taint);
  Solver->register_relation(Edge);
  Solver->register_relation(Path);
  Solver->register_relation(Sanitizer);
  Solver->register_relation(Div);
  Solver->register_relation(Alarm);

  /*
   * Add your code here:
   * Define your analysis rules for taint analysis and add the rules to the
   * solver.
   */
}

void Extractor::addDef(const InstMapTy &InstMap, Value *X, Instruction *L) {
  if (InstMap.find(X) == InstMap.end())
    return;
  unsigned int Arr[2] = {InstMap.at(X), InstMap.at(L)};
  Solver->add_fact(Def, Arr);
}

void Extractor::addUse(const InstMapTy &InstMap, Value *X, Instruction *L) {
  if (Constant *C = dyn_cast<Constant>(X))
    return;
  if (InstMap.find(X) == InstMap.end())
    return;
  unsigned int Arr[2] = {InstMap.at(X), InstMap.at(L)};
  Solver->add_fact(Use, Arr);
}

void Extractor::addDiv(const InstMapTy &InstMap, Value *X, Instruction *L) {
  if (Constant *C = dyn_cast<Constant>(X))
    return;
  if (InstMap.find(X) == InstMap.end())
    return;
  unsigned int Arr[2] = {InstMap.at(X), InstMap.at(L)};
  Solver->add_fact(Div, Arr);
}

void Extractor::addTaint(const InstMapTy &InstMap, Instruction *L) {
  unsigned int Arr[1] = {InstMap.at(L)};
  Solver->add_fact(Taint, Arr);
}

void Extractor::addSanitizer(const InstMapTy &InstMap, Instruction *L) {
  unsigned int Arr[1] = {InstMap.at(L)};
  Solver->add_fact(Sanitizer, Arr);
}

void Extractor::addNext(const InstMapTy &InstMap, Instruction *X,
                        Instruction *Y) {
  unsigned int Arr[2] = {InstMap.at(X), InstMap.at(Y)};
  Solver->add_fact(Next, Arr);
};

/*
 * Implement the following function that collects Datalog facts for each
 * instruction.
 */
void Extractor::extractConstraints(const InstMapTy &InstMap, Instruction *I) {
  /* Add your code here */
  
  /**
   * TODO:
   *   - For each predecessor P of instruction I, add a new fact in the `next` relation
   * 
   *   - For each of the instruction types below, except `AllocaInst`, add appropriate facts in the `def` and `use` relations:
   *     + The fact in the `def` relation must include the Value being defined and the Instruction defining that Value
   *     + The fact in the `use` relation must include any Value being used (eg operands in `BinaryOperator` instructions) and the Instruction using that Value
   * 
   *   - For `BinaryOperator` instructions, additionally add a fact for the `div` relation if the operator is a division
   * 
   *   - For `CallInst` instructions:
   *     + Add the fact to the `def` relation only if it has a non-void return type
   *     + If the instruction is a call to the tainted input, add the appropriate fact to the `taint` relation (check this using the `isTaintedInput()` function)
   *     + If the instruction is a call to the sanitize function, add the appropriate fact to the `sanitizer` relation (check this using the `isSanitizer()` function)
   */

  if (AllocaInst *AI = dyn_cast<AllocaInst>(I)) {
    // do nothing
  } else if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
    // your code goes here

  } else if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
    // your code goes here
    
  } else if (BinaryOperator *BI = dyn_cast<BinaryOperator>(I)) {
    // your code goes here
    
  } else if (CallInst *CI = dyn_cast<CallInst>(I)) {
    // your code goes here
    
  } else if (CastInst *CI = dyn_cast<CastInst>(I)) {
    // your code goes here
    
  } else if (CmpInst *CI = dyn_cast<CmpInst>(I)) {
    // your code goes here
    
  }
}
