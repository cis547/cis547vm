#include "DivZeroAnalysis.h"

#include "Utils.h"

namespace dataflow {

//===----------------------------------------------------------------------===//
// DivZero Analysis Implementation
//===----------------------------------------------------------------------===//

/**
 * PART 1
 * 1. Implement "check" that checks if a given instruction is erroneous or not.
 * 2. Implement "transfer" that computes the semantics of each instruction.
 *    This means that you have to complete "eval" function, too.
 *
 * PART 2
 * 1. Implement "doAnalysis" that stores your results in "InMap" and "OutMap".
 * 2. Implement "flowIn" that joins the memory set of all incoming flows.
 * 3. Implement "flowOut" that flows the memory set to all outgoing flows.
 * 4. Implement "join" to union two Memory objects, accounting for Domain value.
 * 5. Implement "equal" to compare two Memory objects.
 */

bool DivZeroAnalysis::check(Instruction *Inst) {
  /**
   * TODO: Write your code to check if Inst can cause a division by zero.
   *
   * Inst can cause a division by zero if:
   *   Inst is a signed or unsigned division instruction and,
   *   The divisor is either Zero or MaybeZero.
   *
   * Hint: getOrExtract function may be useful to simplify your code.
   */
}

PreservedAnalyses DivZeroAnalysis::run(Function &F, FunctionAnalysisManager &) {
  outs() << "Running " << getAnalysisName() << " on " << F.getName() << "\n";

  // Initializing InMap and OutMap.
  for (inst_iterator Iter = inst_begin(F), End = inst_end(F); Iter != End; ++Iter) {
    auto Inst = &(*Iter);
    InMap[Inst] = new Memory;
    OutMap[Inst] = new Memory;
  }

  // The chaotic iteration algorithm is implemented inside doAnalysis().
  doAnalysis(F);

  // Check each instruction in function F for potential divide-by-zero error.
  for (inst_iterator Iter = inst_begin(F), End = inst_end(F); Iter != End; ++Iter) {
    auto Inst = &(*Iter);
    if (check(Inst))
      ErrorInsts.insert(Inst);
  }

  printMap(F, InMap, OutMap);
  outs() << "Potential Instructions by " << getAnalysisName() << ": \n";
  for (auto Inst : ErrorInsts) {
    outs() << *Inst << "\n";
  }

  for (auto Iter = inst_begin(F), End = inst_end(F); Iter != End; ++Iter) {
    delete InMap[&(*Iter)];
    delete OutMap[&(*Iter)];
  }
  return PreservedAnalyses::all();
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "DivZero", "v0.1", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                    ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "DivZero") {
                    MPM.addPass(createModuleToFunctionPassAdaptor(DivZeroAnalysis()));
                    return true;
                  }
                  return false;
                });
          }};
}
}  // namespace dataflow
