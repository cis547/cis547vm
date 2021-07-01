#include "Utils.h"

#include "llvm/IR/CFG.h"

const char *WhiteSpaces = " \t\n\r";

std::string toString(Value *I) {
  std::string Str;
  raw_string_ostream SS(Str);
  I->print(SS);
  Str.erase(0, Str.find_first_not_of(WhiteSpaces));
  return SS.str();
}

std::vector<Instruction *> getPredecessors(Instruction *I) {
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

bool isTaintedInput(CallInst *CI) {
  return CI->getCalledFunction()->getName().equals("tainted_input");
}

bool isSanitizer(CallInst *CI) {
  return CI->getCalledFunction()->getName().equals("sanitizer");
}
