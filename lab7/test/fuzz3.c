#include <stdio.h>
#include <string.h>

int xval(char *i) {
  if ((strlen(i) % 2) == 0) {
    return 1;
  } else {
    return 0;
  }
}

int main() {
  char input[65536];
  fgets(input, sizeof(input), stdin);
  int x = 0;
  int y = 2;
  int z;

  if (strlen(input) % 7 == 0) {
    z = y / x;
  }

  if (strlen(input) % 13 == 0) {
    z = y / ++x;
  }

  if (xval(input)) {
    z = y / x;
  }

  return 0;
}
