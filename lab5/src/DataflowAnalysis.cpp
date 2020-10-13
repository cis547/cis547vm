#include "DataflowAnalysis.h"

using namespace llvm;

namespace dataflow {

//===----------------------------------------------------------------------===//
// Memory Access
//===----------------------------------------------------------------------===//

const char *WhiteSpaces = " \t\n\r";

std::string variable(Value *V) {
  std::string Code;
  raw_string_ostream SS(Code);
  V->print(SS);
  Code.erase(0, Code.find_first_not_of(WhiteSpaces));
  return Code;
}

std::string address(Value *V) {
  std::string Code;
  raw_string_ostream SS(Code);
  V->print(SS);
  Code.erase(0, Code.find_first_not_of(WhiteSpaces));
  Code = "@(" + Code + ")";
  return Code;
}

//===----------------------------------------------------------------------===//
// Dataflow Analysis Implementation
//===----------------------------------------------------------------------===//

DataflowAnalysis::DataflowAnalysis(char ID) : FunctionPass(ID) {}

void DataflowAnalysis::collectErrorInsts(Function &F) {
  ErrorInsts.clear();
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    if (check(&*I))
      ErrorInsts.insert(&*I);
  }
}

bool DataflowAnalysis::runOnFunction(Function &F) {
  outs() << "Running " << getAnalysisName() << " on " << F.getName() << "\n";
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    InMap[&(*I)] = new Memory;
    OutMap[&(*I)] = new Memory;
  }

  PointerAnalysis *PA = new PointerAnalysis(F);
  doAnalysis(F, PA);

  collectErrorInsts(F);
  outs() << "Potential Instructions by " << getAnalysisName() << ": \n";
  for (auto I : ErrorInsts) {
    outs() << *I << "\n";
  }

  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    delete InMap[&(*I)];
    delete OutMap[&(*I)];
  }
  return false;
}
} // namespace dataflow
