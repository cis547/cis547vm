#include "prelude.h"

int main(int argc, char **argv) {
  int x = tainted_input();
  int y;
  if (argc > 2) {
    x = 0;
    y = sanitizer(x);
  } else {
    y = not_sanitizer(x);
  }
  int z = 4 / y;  // alarm
}
