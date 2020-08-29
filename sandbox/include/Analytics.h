#include "llvm/IR/Module.h"
#include "llvm/ADT/StringRef.h"

using namespace llvm;

namespace analytics {
struct Analytics : public ModulePass {
  static char ID;
  Analytics();

  bool runOnModule(Module &M);

};
} // namespace analytics
