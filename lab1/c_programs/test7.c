#include<stdio.h>
#include<stdlib.h>

void bad() {
    int data;
    int* dataref = &data;
    data = -1;
    {
        int c = 30;
        data = fgetc(stdin) - c;
    }
    {
        int data = *dataref;
        // possible divide by zero
        printf("%d", 100 / (data - 20));
    }
}

void goodG2B() {
    int data;
    int* dataref = &data;

    data = -1;
    data = 7;
    {
        int data = *dataref;
        // not a divide by zero
        printf("%d", 100 / (data - 20));
    }
}

void goodB2G() {
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
            // definitely not a divide by zero
            printf("%d", 100 / (data - 20));
        }
    }
}

void good()
{
    goodG2B();
    goodB2G();
}

int main() {
    printf("Calling good()...\n");
    good();
    printf("Finished good()\n");
    
    printf("Calling bad()...\n");
    bad();
    printf("Finished bad()\n");
    
    return 0;
}
