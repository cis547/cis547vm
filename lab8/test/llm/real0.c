#include <stdlib.h>

int main(int argc, char **argv) {
  int n = atoi(argv[1]);
  int d = n % 8;
  int odd = d | 1;
  int a = 100 / odd; // safe
  int b = 100 / d; // alarm
  return 0;
}
