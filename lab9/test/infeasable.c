#include <stdio.h>

#include "../include/Runtime.h"

int main() {
  int x;
  DSE_Input(x);

  //c is a product of two primes
  int c = 181 * 191;

  int y = 1;

  for(int i=0; i<x; i++){
	y *= 2;
  }

  if (y % c == 17) {
    x = x / (c-c);
  }
  return 0;
}
