#include "prelude.h"

int main(int argc, char **argv) {
  int x = tainted_input();
  int y = sanitizer(x);
  int z = 4 / y;  // safe

  y = not_sanitizer(x);
  z = 4 / y;  // alarm
  return 0;
}
