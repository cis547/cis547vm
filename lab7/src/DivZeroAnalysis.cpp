#include "DivZeroAnalysis.h"

#include "Utils.h"
#include <llvm/Passes/PassPlugin.h>

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
  return false;
}

const auto PASS_NAME = "DivZero";
const auto PASS_DESC = "Divide-by-zero Analysis";

PreservedAnalyses DivZeroAnalysis::run(Module &M, ModuleAnalysisManager &AM) {
  outs() << "Running " << PASS_DESC << " on module " << M.getName() << "\n";

  auto &Context = M.getContext();

  for (auto &F : M) {
    if (F.isDeclaration()) {
      continue;
    }

    outs() << "Running " << getAnalysisName() << " on " << F.getName() << "\n";

    // Initializing InMap and OutMap.
    for (inst_iterator Iter = inst_begin(F), End = inst_end(F); Iter != End; ++Iter) {
      auto Inst = &(*Iter);
      InMap[Inst] = new Memory;
      OutMap[Inst] = new Memory;
    }

    // The chaotic iteration algorithm is implemented inside doAnalysis().
    auto PA = new PointerAnalysis(F);
    doAnalysis(F, PA);

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
  }

  return PreservedAnalyses::all();
}

// Pass registration for the new pass manager
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, "1.0.0", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                    ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == PASS_NAME) {
                    MPM.addPass(DivZeroAnalysis());
                    return true;
                  }
                  return false;
                });
          }};
}

}  // namespace dataflow
