#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHUFFLE(a) ((unsigned)a << 7) | ((unsigned)a >> 25)

int hash(int x) {
  int y = ~x;

  const int MASK_A = 0xF03AD267;
  y = y ^ MASK_A;

  unsigned u = SHUFFLE(y);

  int w = (y ^ (u << 1)) ^ (u << 1);
  w = ((w & u) | (w & ~u)) ^ 0;

  for (int i = 0; i < 4; ++i) {
    w = ((unsigned)w << 1) ^ (w >> 1);
    w ^= (i * 65342);
    w &= ~0;
  }

  unsigned v = w ^ u;
  v = ((v ^ w) >> 7) | ((unsigned)(v ^ w) << (32 - 7));

  y = ~(v ^ MASK_A);

  y = ((y | 0) & ~0);

  return y;
}

int main() {
  int a = 0;
  char s[20];
  fgets(s, sizeof(s), stdin);
  int l = 0;
  int r = hash(getchar() - '0');
  int w = l / ((unsigned)r << 2);

  return 0;
}
