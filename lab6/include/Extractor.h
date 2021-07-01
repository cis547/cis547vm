#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "z3++.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include <set>

#include "Utils.h"

using namespace llvm;

using InstMapTy = std::map<Value *, unsigned int>;
using DefMapTy = std::map<Value *, std::set<Value *>>;

class Extractor {
public:
  Extractor() {
    Solver = new z3::fixedpoint(C);
    Params = new z3::params(C);
    Params->set("engine", "datalog");
    Solver->set(*Params);
  }

  ~Extractor() {
    delete Solver;
    delete Params;
  }

  void initialize();
  z3::fixedpoint *getSolver() { return Solver; }
  z3::context &getContext() { return C; }
  z3::check_result query(unsigned int N) {
    z3::expr Q = Alarm(C.bv_val(N, 32));
    return Solver->query(Q);
  }

  void addNext(const InstMapTy &InstMap, Instruction *X, Instruction *Y);
  void addDef(const InstMapTy &InstMap, Value *X, Instruction *L);
  void addUse(const InstMapTy &InstMap, Value *X, Instruction *L);
  void addDiv(const InstMapTy &InstMap, Value *X, Instruction *L);
  void addTaint(const InstMapTy &InstMap, Instruction *L);
  void addSanitizer(const InstMapTy &InstMap, Instruction *L);

  void extractConstraints(const InstMapTy &InstMap, Instruction *I);

  void printTuple(std::string Name, Value *V1, Value *V2) {
    std::cout << Name << "(\"" << toString(V1) << "\", \"" << toString(V2)
              << "\")" << std::endl;
  }

  void print(InstMapTy &InstMap) {
    std::cout << "=== Reaching Definition (Out) ===" << std::endl;
    for (auto &V1 : InstMap) {
      for (auto &V2 : InstMap) {
        z3::expr Q = Out(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat) {
          printTuple("Out", V1.first, V2.first);
        }
      }
    }
    std::cout << "=== Kill ===" << std::endl;
    for (auto &V1 : InstMap) {
      for (auto &V2 : InstMap) {
        z3::expr Q = Kill(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat) {
          printTuple("Kill", V1.first, V2.first);
        }
      }
    }
    std::cout << "=== Def ===" << std::endl;
    for (auto &V1 : InstMap) {
      for (auto &V2 : InstMap) {
        z3::expr Q = Def(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat) {
          printTuple("Def", V1.first, V2.first);
        }
      }
    }
    std::cout << "=== Use ===" << std::endl;
    for (auto &V1 : InstMap) {
      for (auto &V2 : InstMap) {
        z3::expr Q = Use(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat) {
          printTuple("Use", V1.first, V2.first);
        }
      }
    }
    std::cout << "=== Edge ===" << std::endl;
    for (auto &V1 : InstMap) {
      for (auto &V2 : InstMap) {
        z3::expr Q = Edge(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat) {
          printTuple("Edge", V1.first, V2.first);
        }
      }
    }
    std::cout << "=== Path ===" << std::endl;
    for (auto &V1 : InstMap) {
      for (auto &V2 : InstMap) {
        z3::expr Q = Path(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat) {
          printTuple("Path", V1.first, V2.first);
        }
      }
    }
  }

private:
  std::map<Value *, std::set<Value *>> DefMap;

  z3::context C;
  z3::fixedpoint *Solver;
  z3::params *Params;
  z3::check_result Result;
  z3::sort LLVMInst = C.bv_sort(32);

public:
  /* Relations for Def and Use */
  z3::func_decl Def = C.function("Def", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Use = C.function("Use", LLVMInst, LLVMInst, C.bool_sort());

  /* Relations for Reaching Definition */
  z3::func_decl Kill = C.function("Kill", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Next = C.function("Next", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl In = C.function("In", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Out = C.function("Out", LLVMInst, LLVMInst, C.bool_sort());

  /* Relations for Taint Analysis */
  z3::func_decl Taint = C.function("Taint", LLVMInst, C.bool_sort());
  z3::func_decl Edge = C.function("Edge", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Path = C.function("Path", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Sanitizer = C.function("Sanitizer", LLVMInst, C.bool_sort());
  z3::func_decl Div = C.function("Div", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Alarm = C.function("Alarm", LLVMInst, C.bool_sort());
};

#endif // EXTRACTOR_H
