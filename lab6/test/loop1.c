#include "prelude.h"

int main(int argc, char **argv) {
  int x;
  int y = tainted_input();
  x = 0;
  while (x > 0) {
    y = sanitizer(y);
  }
  int z = 4 / y; // alarm
}
