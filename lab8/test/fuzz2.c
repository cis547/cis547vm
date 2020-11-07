#include <stdio.h>
#include <string.h>

int main() {
  char input[65536];
  fgets(input, sizeof(input), stdin);
  int x = 0;
  int y = 2;
  int z;
  if (strlen(input) > 10 && input[5] == 'a') {
    z = y / x;
  }

  return 0;
}
