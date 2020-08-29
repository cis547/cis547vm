#include <stdio.h>

int main() {
  int i, a = 0, b = getchar();
  for (i = 10000; i > 0; i--)
    printf("%d\n", b * b / (i - b)); // Divide by zero
  return 0;
}
