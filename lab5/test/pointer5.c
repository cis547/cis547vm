#include <stdio.h>

int f() {
  int in = getchar();
  int t = 10;
  int *si = &in;
  int *st = &t;
  int **get = &si;
  if (in - t > 0){
	  get = &si;
  }else{
	  get = &st;
  }
  return in / **get;
} 
