/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE369_Divide_by_Zero__int_fgets_divide_33.cpp
Label Definition File: CWE369_Divide_by_Zero__int.label.xml
Template File: sources-sinks-33.tmpl.cpp
*/
/*
 * @description
 * CWE: 369 Divide by Zero
 * BadSource: fgets Read data from the console using fgets()
 * GoodSource: Non-zero
 * Sinks: divide
 *    GoodSink: Check for zero before dividing
 *    BadSink : Divide a constant by data
 * Flow Variant: 33 Data flow: use of a C++ reference to data within the same function
 *
 * */
#include<stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_ARRAY_SIZE (3 * sizeof(data) + 2)

void bad()
{
    int data;
    int *dataRef = &data;
    /* Initialize data */
    data = -1;
    {
        /* POTENTIAL FLAW: Read data from the console using fgets() */
        data = fgetc(stdin);
    }
    {
        int data = *dataRef;
        /* POTENTIAL FLAW: Possibly divide by zero */
        printf("%d", 100 / data);
    }
}

/* goodG2B() uses the GoodSource with the BadSink */
static void goodG2B()
{
    int data;
    int *dataRef = &data;
    /* Initialize data */
    data = -1;
    data = 7;
    {
        int data = *dataRef;

        // not a divide by zero
        printf("%d", 100 / data);
    }
}

/* goodB2G() uses the BadSource with the GoodSink */
static void goodB2G()
{
    int data;
    int *dataRef = &data;
    /* Initialize data */
    data = -1;
    {
        data = fgetc(stdin);
    }
    {
        int data = *dataRef;
        /* FIX: test for a zero denominator */
        if( data != 0 )
        {
            // not a divide by zero
            printf("%d", 100 / data);
        }
        else
        {
            printf("This would result in a divide by zero\n");
        }
    }
}

void good()
{
    goodG2B();
    goodB2G();
}

/* Below is the main(). It is only used when building this testcase on
   its own for testing or for building a binary to use in testing binary
   analysis tools. It is not used when compiling all the testcases as one
   application, which is how source code analysis tools are tested. */
// #ifdef INCLUDEMAIN

// using namespace CWE369_Divide_by_Zero__int_fgets_divide_33; /* so that we can use good and bad easily */

int main(int argc, char * argv[])
{
    /* seed randomness */
    // srand( (unsigned)time(NULL) );
    
    printf("Calling good()...\n");
    good();
    printf("Finished good()\n");
    
    printf("Calling bad()...\n");
    bad();
    printf("Finished bad()\n");
    
    return 0;
}

// #endif