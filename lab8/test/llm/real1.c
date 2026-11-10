#include <stdio.h>

int main() {
  int c = getchar();
  int k;
  if (c > 64) {
    k = c % 10;
  } else {
    k = c == 0 ? 1 : c;
  }
  int r = 100 / k; // alarm
  return 0;
}
