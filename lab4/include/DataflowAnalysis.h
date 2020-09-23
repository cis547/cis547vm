#ifndef DATAFLOW_ANALYSIS_H
#define DATAFLOW_ANALYSIS_H

#include "llvm/ADT/SetVector.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <iterator>
#include <map>
#include <string>

#include "Domain.h"

using namespace llvm;

namespace dataflow {

using Memory = std::map<std::string, Domain *>;

std::string variable(Value *);

struct DataflowAnalysis : public FunctionPass {
  ValueMap<Instruction *, Memory *> InMap;
  ValueMap<Instruction *, Memory *> OutMap;
  SetVector<Instruction *> ErrorInsts;

  DataflowAnalysis(char ID);
  void collectErrorInsts(Function &F);
  bool runOnFunction(Function &F) override;

protected:
  virtual void transfer(Instruction *I, const Memory *In, Memory *NOut) = 0;
  virtual void doAnalysis(Function &F) = 0;
  virtual bool check(Instruction *I) = 0;
  virtual std::string getAnalysisName() = 0;
};


inline bool isInput(Instruction *I) {
  if (CallInst *CI = dyn_cast<CallInst>(I)) {
    return CI->getCalledFunction() && (CI->getCalledFunction()->getName().equals("getchar") || CI->getCalledFunction()->getName().equals("fgetc"));
  } else {
    return false;
  }
}

/**
 * Get the Predecessors of a given instruction in the control-flow graph.
 */
inline std::vector<Instruction *> getPredecessors(Instruction *I) {
  std::vector<Instruction *> Ret;
  BasicBlock *BB = I->getParent();
  for (BasicBlock::reverse_iterator It = BB->rbegin(), E = BB->rend(); It != E;
       ++It) {
    if (&(*It) == I) {
      ++It;
      if (It == E) {
        for (pred_iterator Pre = pred_begin(BB), BE = pred_end(BB); Pre != BE;
             ++Pre)
          Ret.push_back(&(*((*Pre)->rbegin())));
      } else {
        Ret.push_back(&(*It));
      }
      break;
    }
  }
  return Ret;
}

/**
 * Get the successors of a given instruction in the control-flow graph.
 */
inline std::vector<Instruction *> getSuccessors(Instruction *I) {
  std::vector<Instruction *> Ret;
  BasicBlock *BB = I->getParent();
  for (BasicBlock::iterator It = BB->begin(), E = BB->end(); It != E; ++It) {
    if (&(*It) == I) {
      ++It;
      if (It == E) {
        for (succ_iterator Succ = succ_begin(BB), BS = succ_end(BB); Succ != BS;
             ++Succ)
          Ret.push_back(&(*((*Succ)->begin())));
      } else {
        Ret.push_back(&(*It));
      }
      break;
    }
  }
  return Ret;
}
} // namespace dataflow

#endif // DATAFLOW_ANALYSIS_H
