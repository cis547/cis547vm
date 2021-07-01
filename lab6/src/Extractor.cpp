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
}
