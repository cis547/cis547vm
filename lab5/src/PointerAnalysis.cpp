#include "PointerAnalysis.h"

#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"

#include "DivZeroAnalysis.h"

namespace dataflow {

//===----------------------------------------------------------------------===//
// Pointer Analysis Implementation
//===----------------------------------------------------------------------===//

/*
 * 1. "PointerAnalysis" stores your results in "PointsTo".
 * 2. "alias" checks whether two pointers may alias each other.
 */

void transfer(Instruction *I, PointsToInfo &PointsTo) {
  if (AllocaInst *AI = dyn_cast<AllocaInst>(I)) {
    std::string Pointer = variable(AI);
    PointsToSet &S = PointsTo[variable(AI)];
    S.insert(address(AI));
  } else if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
    if (!SI->getValueOperand()->getType()->isPointerTy())
      return;
    Value *Pointer = SI->getPointerOperand();
    Value *Value = SI->getValueOperand();
    PointsToSet &L = PointsTo[variable(Pointer)];
    PointsToSet &R = PointsTo[variable(Value)];
    for (auto &I : L) {
      PointsToSet &S = PointsTo[I];
      PointsToSet Result;
      std::set_union(S.begin(), S.end(), R.begin(), R.end(),
                     std::inserter(Result, Result.begin()));
      PointsTo[I] = Result;
    }
  } else if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
    if (!LI->getType()->isPointerTy())
      return;
    std::string Variable = variable(LI->getPointerOperand());
    PointsToSet &R = PointsTo[Variable];
    PointsToSet &L = PointsTo[variable(LI)];
    PointsToSet Result;
    for (auto &I : R) {
      PointsToSet &S = PointsTo[I];
      std::set_union(S.begin(), S.end(), Result.begin(), Result.end(),
                     std::inserter(Result, Result.begin()));
    }
    PointsTo[variable(LI)] = Result;
  }
}

int countFacts(PointsToInfo &PointsTo) {
  int N = 0;
  for (auto &I : PointsTo) {
    N += I.second.size();
  }
  return N;
}

void print(std::map<std::string, PointsToSet> &PointsTo) {
  errs() << "Pointer Analysis Results:\n";
  for (auto &I : PointsTo) {
    errs() << "  " << I.first << ": { ";
    for (auto &J : I.second) {
      errs() << J << "; ";
    }
    errs() << "}\n";
  }
  errs() << "\n";
}

PointerAnalysis::PointerAnalysis(Function &F) {
  int NumOfOldFacts = 0;
  int NumOfNewFacts = 0;
  while (true) {
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
      transfer(&*I, PointsTo);
    }
    NumOfNewFacts = countFacts(PointsTo);
    if (NumOfOldFacts < NumOfNewFacts)
      NumOfOldFacts = NumOfNewFacts;
    else
      break;
  }
  print(PointsTo);
}

bool PointerAnalysis::alias(std::string &Ptr1, std::string &Ptr2) const {
  if (PointsTo.find(Ptr1) == PointsTo.end() ||
      PointsTo.find(Ptr2) == PointsTo.end())
    return false;
  const PointsToSet &S1 = PointsTo.at(Ptr1);
  const PointsToSet &S2 = PointsTo.at(Ptr2);
  PointsToSet Inter;
  std::set_intersection(S1.begin(), S1.end(), S2.begin(), S2.end(),
                        std::inserter(Inter, Inter.begin()));
  return !Inter.empty();
}

}; // namespace dataflow
