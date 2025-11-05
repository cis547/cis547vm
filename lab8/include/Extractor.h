#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "Utils.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"

#include <fstream>
#include <iostream>
#include <set>
#include <map>

using namespace llvm;

using InstMapTy = std::map<Value *, unsigned int>;
using DefMapTy = std::map<Value *, std::set<Value *>>;

class Extractor {
 public:
  Extractor(const std::string OutDir) {
    DefFile.open(OutDir + "/def.facts");
    UseFile.open(OutDir + "/use.facts");
    NextFile.open(OutDir + "/next.facts");
    TaintFile.open(OutDir + "/taint.facts");
    SanitizerFile.open(OutDir + "/sanitizer.facts");
    DivFile.open(OutDir + "/div.facts");
  }

  ~Extractor() {
    DefFile.close();
    UseFile.close();
    NextFile.close();
    TaintFile.close();
    SanitizerFile.close();
    DivFile.close();
  }

  void addNext(const InstMapTy &InstMap, Instruction *X, Instruction *Y);
  void addDef(const InstMapTy &InstMap, Value *X, Instruction *L);
  void addUse(const InstMapTy &InstMap, Value *X, Instruction *L);
  void addDiv(const InstMapTy &InstMap, Value *X, Instruction *L);
  void addTaint(const InstMapTy &InstMap, Instruction *L);
  void addSanitizer(const InstMapTy &InstMap, Instruction *L);

  void extractConstraints(const InstMapTy &InstMap, Instruction *I);

  void printTuple(std::string Name, Value *V1, Value *V2) {
    std::cerr << Name << "(\"" << toString(V1) << "\", \"" << toString(V2) << "\")"
              << std::endl;
  }

 private:
  std::map<Value *, std::set<Value *>> DefMap;
  std::ofstream DefFile, UseFile, NextFile, TaintFile, SanitizerFile, DivFile;
};

#endif  // EXTRACTOR_H
