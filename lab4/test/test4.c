#include <stdio.h>
#include <string.h>

int main() {
  int i;
  int x = 0;
  int y = 2;
  int z;
  while ((i = getchar()) - '0' >= 0) {
    if (i == 'y') {
      z = y / x;
    }
  }
  return 0;
}
