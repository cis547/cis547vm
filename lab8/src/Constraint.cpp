#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/SourceMgr.h"
#include <fstream>
#include <sys/stat.h>

#include "Extractor.h"

#define ARG_EXIST_CHECK(Name, Arg)                                             \
  {                                                                            \
    struct stat Buffer;                                                        \
    if (stat(Arg, &Buffer)) {                                                  \
      fprintf(stderr, "%s not found\n", Arg);                                  \
      return 1;                                                                \
    }                                                                          \
  }                                                                            \
  std::string Name(Arg);

using namespace llvm;

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("usage: %s [target] [output dir]\n", argv[0]);
    return 1;
  }

  ARG_EXIST_CHECK(Target, argv[1]);
  ARG_EXIST_CHECK(OutDir, argv[2]);

  LLVMContext Context;
  SMDiagnostic Err;

  auto Mod = parseAssemblyFile(Target, Err, Context);

  if (!Mod) {
    Err.print(argv[0], errs());
    return 1;
  }

  Extractor Ext(OutDir);

  InstMapTy InstMap;
  unsigned int Counter = 0;
  for (auto &F : *Mod) {
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; I++) {
      InstMap[&*I] = Counter++;
    }
  }

  for (auto &F : *Mod) {
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; I++) {
      Ext.extractConstraints(InstMap, &*I);
    }
  }
}
