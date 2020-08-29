#include <stdio.h>

int main(){
  int y = 1, z = 1;
  int un;
  scanf("%d", &un); 
  int x = un + 1;
  z = y / x; // Divide by zero
  return 0;
}
