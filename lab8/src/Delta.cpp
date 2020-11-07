#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <string.h>

#include "Utils.h"

/*
 * Implement the delta-debugging algorithm.
 */
std::string delta(std::string &Target, std::string &Input) {
  /* Add your code here */
  return Input;
}

// ./delta [exe file] [crashing input file]
int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Invalid usage\n");
    return 1;
  }

  struct stat Buffer;
  if (stat(argv[1], &Buffer)) {
    fprintf(stderr, "%s not found\n", argv[1]);
    return 1;
  }

  if (stat(argv[2], &Buffer)) {
    fprintf(stderr, "%s not found\n", argv[2]);
    return 1;
  }

  std::string Target(argv[1]);
  std::string InputFile(argv[2]);
  std::string Input = readOneFile(InputFile);
  if (!runTarget(Target, Input)) {
    fprintf(stderr, "Sanity check failed: the program does not crash with the "
                    "initial input\n");
    return 1;
  }

  std::string DeltaOutput = delta(Target, Input);

  std::string Path = InputFile + ".delta";
  std::ofstream OutFile(Path);
  OutFile << DeltaOutput;
  OutFile.close();

  return 0;
}
