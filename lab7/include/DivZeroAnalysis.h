#ifndef DIV_ZERO_ANALYSIS_H
#define DIV_ZERO_ANALYSIS_H

#include "Domain.h"
#include "PointerAnalysis.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <string>

namespace dataflow {

using Memory = std::map<std::string, Domain *>;

struct DivZeroAnalysis : public PassInfoMixin<DivZeroAnalysis> {
  static char ID;
  std::map<Instruction *, Memory *> InMap;
  std::map<Instruction *, Memory *> OutMap;
  SetVector<Instruction *> ErrorInsts;

  /**
   * This function is called for each module M in the input C program
   * that the compiler encounters during a pass.
   * You do not need to modify this function.
   */
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);

 protected:
  /**
   * This function creates a transfer function that updates the Out Memory based
   * on In Memory and the instruction type/parameters.
   */
  void transfer(Instruction *I,
      const Memory *In,
      Memory &NOut,
      PointerAnalysis *PA,
      SetVector<Value *> PointerSet);

  /**
   * @brief This function implements the chaotic iteration algorithm using
   * flowIn(), transfer(), and flowOut().
   *
   * @param F The function to be analyzed.
   */
  void doAnalysis(Function &F, PointerAnalysis *PA);

  /**
   * @brief Flow the abstract domains from all predecessors of Inst into the In
   * Memory object for Inst.
   *
   * @param Inst Instruction to flow In Memory for.
   * @param InMem InMemory object of Inst to populate.
   */
  void flowIn(Instruction *Inst, Memory *InMem);

  /**
   * @brief Merge the previous Out Memory of Inst with the current Out Memory
   * for each instruction to update the OutMap and WorkSet as needed.
   *
   * @param Inst Instruction to flow Out Memory for.
   * @param Pre Previous OutMemory of Inst.
   * @param Post Current OutMemory of Inst.
   * @param WorkSet WorkSet
   */
  void flowOut(
      Instruction *Inst, Memory *Pre, Memory *Post, SetVector<Instruction *> &WorkSet);

  /**
   * Can the Instruction Inst incurr a divide by zero error?
   *
   * @param Inst Instruction to check.
   * @return true if the instruction can incur a divide by zero error.
   */
  bool check(Instruction *Inst);

  std::string getAnalysisName() {
    return "DivZero";
  }
};
}  // namespace dataflow

#endif  // DIV_ZERO_ANALYSIS_H
