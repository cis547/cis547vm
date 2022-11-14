#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

#include "z3++.h"

#include "Strategy.h"
#include "SymbolicInterpreter.h"

#define ARG_EXIST_CHECK(Name, Arg)                                             \
  {                                                                            \
    struct stat Buffer;                                                        \
    if (stat(Arg, &Buffer)) {                                                  \
      fprintf(stderr, "%s not found\n", Arg);                                  \
      return 1;                                                                \
    }                                                                          \
  }                                                                            \
  std::string Name(Arg);

z3::context Ctx;
z3::solver Solver(Ctx);

void storeInput() {
  std::ofstream OS(InputFile);
  z3::model Model = Solver.get_model();
  for (int I = 0; I < Model.size(); I++) {
    const z3::func_decl E = Model[I];
    z3::expr Input = Model.get_const_interp(E);
    if (Input.kind() == Z3_NUMERAL_AST) {
      int I = Input.get_numeral_int();
      OS << E.name() << "," << I << std::endl;
    }
  }
}

void printNewPathCondition(z3::expr_vector &Vec) {
  std::ofstream Log;
  Log.open(LogFile, std::ofstream::out | std::ofstream::app);
  Log << std::endl;
  Log << "=== New Path Condition ===" << std::endl;
  for (auto E : Vec) {
    Log << E << std::endl;
  }
}

void generateInput() {
  z3::expr_vector Vec = Ctx.parse_file(FormulaFile);

  while (true) {
    searchStrategy(Vec);

    for (const auto &E : Vec) {
      Solver.add(E);
    }
    z3::check_result Result = Solver.check();
    if (Result == z3::sat) {
      storeInput();
      printNewPathCondition(Vec);
      break;
    }
  }
}

/**
 * Usage:
 * ./dse [target] (iterations)
 */
int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " [target] (iterations)" << std::endl;
    return 1;
  }

  ARG_EXIST_CHECK(Target, argv[1]);

  int MaxIter = INT_MAX;
  if (argc == 3) {
    MaxIter = atoi(argv[2]);
  }

  struct stat Buffer;
  int Iter = 0;
  while (Iter < MaxIter) {
    int Ret = std::system(Target.c_str());
    if (Ret) {
      std::cout << "Crashing input found (" << Iter << " iters)" << std::endl;
      break;
    }
    if (stat(FormulaFile, &Buffer)) {
      std::cerr << FormulaFile << " not found" << std::endl;
      return 1;
    }
    generateInput();
    Iter++;
  }
}
