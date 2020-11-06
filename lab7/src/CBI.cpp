#include <cstdlib>
#include <string>
#include <unistd.h>

#include "Utils.h"

/*
 * Implement your CBI report generator.
 */
void generateReport() { /* Add your code here */ }

// ./CBI [exe file] [fuzzer output dir]
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
  std::string OutDir(argv[2]);

  generateLogFiles(Target, OutDir);
  generateReport();
  printReport();
  return 0;
}
