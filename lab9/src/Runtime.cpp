#include <iostream>

#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"

#include "SymbolicInterpreter.h"

extern SymbolicInterpreter SI;

/*
 * Implement your transfer functions.
 * 
 * We have filled out some of the function implementation. Feel free to change this,
 * or anything in this file except the function declarations
 */

z3::expr eval(z3::expr &E) {
  if (E.kind() == Z3_NUMERAL_AST) {
    return E;
  } else {
    MemoryTy Mem = SI.getMemory();
    Address Register(E);
    if (Mem.find(Register) != Mem.end()) {
      return Mem.at(Register);
    } else {
      std::cout << "Warning: Cannot find register " << Register << " in memory "
                << std::endl;
      return E;
    }
  }
}

extern "C" void __DSE_Alloca__(int R, int *Ptr) {
  MemoryTy &Mem = SI.getMemory();
  Address Register(R);
  z3::expr SE = SI.getContext().int_val((uintptr_t)Ptr);
  Mem.insert(std::make_pair(Register, SE));
}

extern "C" void __DSE_Store__(int *X) {
    MemoryTy &Mem = SI.getMemory();
    Address Addr(X);
    z3::expr SE = eval(SI.getStack().top());
    SI.getStack().pop();
    Mem.erase(Addr);
    Mem.insert(std::make_pair(Addr, SE));
}

extern "C" void __DSE_Load__(int Y, int *X) { /* Add your code here */ }

extern "C" void __DSE_ICmp__(int R, int Op) { /* Add your code here */ }

extern "C" void __DSE_BinOp__(int R, int Op) { /* Add your code here */ }
