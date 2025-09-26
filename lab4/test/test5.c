#include <stdio.h>
#include <string.h>

int main() {
  int i;
  int j = 0;
  int x = 0;
  int y = 2;
  int z;
  while ((i = getchar()) - '0' >= 0) {
    if (i == 'y') {
      j = 1;
    }
    if (j == 1 && i == 'x') {
      z = y / x;
    }
  }
  return 0;
}
