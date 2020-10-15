#include <stdio.h>

int main(){
	int a = 1;
	int b = 0;
	int* p = &a;
	int** q = &p;

	p = &b;

	int s = b / **q; //div by zero
	return 0;
}
