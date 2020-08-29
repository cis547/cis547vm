#include <stdio.h>

int main(){
	int a = 0;
	int b = 1;
	int* p = &a;
	int* q = &b;

	*p = *q;

	int s = b / *p; //not a div by zero
	return 0;
}
