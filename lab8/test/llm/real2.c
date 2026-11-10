#include <stdlib.h>

int main() {
  int x = atoi(getenv("SIZE"));
  int y = x < 0 ? -x : x;
  y = (y & 15) + 1;
  int total = 0;
  int n = 3;
  while (n > 0) {
    total = total + 100 / y; // safe
    n--;
  }
  return 0;
}
