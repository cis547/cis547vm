#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/SourceMgr.h"
#include <fstream>

#include "Extractor.h"

using namespace llvm;

int main(int argc, char **argv) {
  if (argc > 3) {
    errs() << "Expected an argument - IR file name\n";
    exit(1);
  }

  LLVMContext Context;
  SMDiagnostic Err;
  StringRef FileName(argv[1]);

  std::unique_ptr<Module> Mod = parseAssemblyFile(FileName, Err, Context);

  if (!Mod) {
    Err.print(argv[0], errs());
    return 1;
  }

  Extractor Ext;
  Ext.initialize();
  z3::fixedpoint *Solver = Ext.getSolver();
  z3::context &C = Ext.getContext();

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

  if (argc == 3 && !strcmp(argv[2], "-d"))
    Ext.print(InstMap);

  std::cout << "Potential divide-by-zero points:" << std::endl;
  for (auto &Entry : InstMap) {
    z3::check_result R = Ext.query(Entry.second);
    if (R == z3::sat)
      std::cout << toString(Entry.first) << std::endl;
  }
}
