#ifndef DIV_ZERO_ANALYSIS_H
#define DIV_ZERO_ANALYSIS_H

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

#include "PointerAnalysis.h"
#include "Domain.h"


namespace dataflow {

using Memory = std::map<std::string, Domain *>;

std::string variable(Value *);
std::string address(Value *);

// struct DataflowAnalysis : public FunctionPass {

//   DataflowAnalysis(char ID);
// };


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
struct DivZeroAnalysis : public FunctionPass {
  static char ID;
  DivZeroAnalysis() : FunctionPass(ID) {}
  ValueMap<Instruction *, Memory *> InMap;
  ValueMap<Instruction *, Memory *> OutMap;
  SetVector<Instruction *> ErrorInsts;

  void collectErrorInsts(Function &F);
  bool runOnFunction(Function &F) override;

protected:
  void transfer(Instruction *I, const Memory *In, Memory *NOut,
                        PointerAnalysis *PA, SetVector<Value *> PointerSet);

  void doAnalysis(Function &F, PointerAnalysis *PA);

  void flowIn(Instruction *I, Memory *In);

  void flowOut(Instruction *I, Memory *In, Memory *NOut,  SetVector <Instruction *> &);

  bool check(Instruction *I);

  std::string getAnalysisName() { return "DivZero"; }
};
} // namespace dataflow

#endif // DIV_ZERO_ANALYSIS_H
