#include "prelude.h"

int main() {
  int x = tainted_input();
  int y = x;
  int z = 4 / y;  // alarm

  return 0;
}
