/* TEMPLATE GENERATED TESTCASE FILE
Filename: CWE369_Divide_by_Zero__int_rand_divide_01.c
Label Definition File: CWE369_Divide_by_Zero__int.label.xml
Template File: sources-sinks-01.tmpl.c
*/
/*
 * @description
 * CWE: 369 Divide by Zero
 * BadSource: rand Set data to result of rand(), which may be zero
 * GoodSource: Non-zero
 * Sinks: divide
 *    GoodSink: Check for zero before dividing
 *    BadSink : Divide a constant by data
 * Flow Variant: 01 Baseline
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void CWE369_Divide_by_Zero__int_rand_divide_01_bad()
{
    int data;
    int* dp_1 = &data;
    int* dp_2 = &data;
    
    /* Initialize data */
    data = -1;
    /* POTENTIAL FLAW: Set data to a random value */
    data = fgetc(stdin);

    // potential div by zero
    printf("%d", 100 / data);

    *dp_2 = 100;
    /* POTENTIAL FLAW: Not divide by zero */
    printf("%d", 100 / data);
}

/* goodG2B uses the GoodSource with the BadSink */
static void goodG2B()
{
    int data;
    int* dp3 = &data;
    /* Initialize data */
    data = 0;
    /* FIX: Use a value not equal to zero */
    *dp3 = 7;
    /* POTENTIAL FLAW: Possibly divide by zero */
    printf("%d", 100 / data);
}

/* goodB2G uses the BadSource with the GoodSink */
static void goodB2G()
{
    int data;
    /* Initialize data */
    data = -1;
    /* POTENTIAL FLAW: Set data to a random value */
    data = fgetc(stdin);
    /* FIX: test for a zero denominator */
    if( data != 0 )
    {
        printf("%d", 100 / data);
    }
    else
    {
        printf("This would result in a divide by zero");
    }
}

void CWE369_Divide_by_Zero__int_rand_divide_01_good()
{
    goodG2B();
    goodB2G();
}

/* Below is the main(). It is only used when building this testcase on
   its own for testing or for building a binary to use in testing binary
   analysis tools. It is not used when compiling all the testcases as one
   application, which is how source code analysis tools are tested. */

int main(int argc, char * argv[])
{
    /* seed randomness */

    printf("Calling good()...");
    CWE369_Divide_by_Zero__int_rand_divide_01_good();
    printf("Finished good()");

    printf("Calling bad()...");
    CWE369_Divide_by_Zero__int_rand_divide_01_bad();
    printf("Finished bad()");

    return 0;
}
