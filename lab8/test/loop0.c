#include "prelude.h"

int main(int argc, char **argv) {
  int x;
  int y;
  x = 0;
  while (argc > 1) {
    x = tainted_input();
    y = x;
    argc--;
  }
  int z = 4 / y;  // alarm
}
