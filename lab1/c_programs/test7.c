#include<stdio.h>
#include<stdlib.h>

void taz() {
    int data;
    int* dataref = &data;
    data = -1;
    {
        int c = 30;
        data = fgetc(stdin) - c;
    }
    {
        int data = *dataref;
        printf("%d", 100 / (data - 20));
    }
}

void bar() {
    int data;
    int* dataref = &data;

    data = -1;
    data = 7;
    {
        int data = *dataref;
        printf("%d", 100 / (data - 20));
    }
}

void baz() {
    int data;
    int* dataref = &data;

    data = -1;
    {
        int c = 30;
        data = fgetc(stdin) - c;
    }
    {
        int data = *dataref;
        if(data < 0) {
            printf("%d", 100 / (data - 20));
        }
    }
}

void foo()
{
    bar();
    baz();
}

int main() {
    printf("Entering foo()...\n");
    foo();
    printf("Finished foo()\n");
    
    printf("Entering taz()...\n");
    taz();
    printf("Finished taz()\n");
    
    return 0;
}
