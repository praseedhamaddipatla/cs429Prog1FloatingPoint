#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// include for whole file giving error, only declare method being used
int conversion(int argc, char *argv[]);

void runTest(char *args[], int argc, char *expected, char *testName)
{
    // redirect output
    FILE *old = stdout;
    freopen("output.txt", "w", stdout);

    conversion(argc, args);

    // restore output
    fflush(stdout);
    freopen("/dev/tty", "w", stdout);

    // read result
    FILE *f = fopen("output.txt", "r");
    if (!f)
    {
        printf("Could not open output.txt\n");
        return;
    }

    char result[200] = {0};
    fgets(result, 200, f);
    fclose(f);

    if (strcmp(result, expected) == 0)
    {
        printf("PASS: %s\n", testName);
    }
    else
    {
        printf("FAIL: %s\n", testName);
        printf("    expected: %s\n", expected);
        printf("    actual: %s\n", result);
    }
}

void test32_DecToIEEE()
{

    char *a1[] = {"test", "0.0", "-s"};
    runTest(a1, 3, "00000000000000000000000000000000\n", "+0 to single");

    char *a2[] = {"test", "-0.0", "-s"};
    runTest(a2, 3, "10000000000000000000000000000000\n", "-0 to single");

    char *a3[] = {"test", "1.0", "-s"};
    runTest(a3, 3, "00111111100000000000000000000000\n", "1.0 to single");

    char *a4[] = {"test", "-1.0", "-s"};
    runTest(a4, 3, "10111111100000000000000000000000\n", "-1.0 to single");

    char *a5[] = {"test", "2.0", "-s"};
    runTest(a5, 3, "01000000000000000000000000000000\n", "2.0 to single");

    char *a6[] = {"test", "0.5", "-s"};
    runTest(a6, 3, "00111111000000000000000000000000\n", "0.5 to single");

    char *a7[] = {"test", "13.25", "-s"};
    runTest(a7, 3, "01000001010101000000000000000000\n", "13.25 to single");

    char *a8[] = {"test", "-13.25", "-s"};
    runTest(a8, 3, "11000001010101000000000000000000\n", "-13.25 to single");
}

void test64_DecToIEEE()
{

    char *a1[] = {"test", "0.0", "-d"};
    runTest(a1, 3,
            "0000000000000000000000000000000000000000000000000000000000000000\n",
            "+0 to double");

    char *a2[] = {"test", "-0.0", "-d"};
    runTest(a2, 3,
            "1000000000000000000000000000000000000000000000000000000000000000\n",
            "-0 to double");

    char *a3[] = {"test", "1.0", "-d"};
    runTest(a3, 3,
            "0011111111110000000000000000000000000000000000000000000000000000\n",
            "1.0 to double");

    char *a4[] = {"test", "-1.0", "-d"};
    runTest(a4, 3,
            "1011111111110000000000000000000000000000000000000000000000000000\n",
            "-1.0 to double");

    char *a5[] = {"test", "2.0", "-d"};
    runTest(a5, 3,
            "0100000000000000000000000000000000000000000000000000000000000000\n",
            "2.0 to double");

    char *a6[] = {"test", "0.5", "-d"};
    runTest(a6, 3,
            "0011111111100000000000000000000000000000000000000000000000000000\n",
            "0.5 to double");

    char *a7[] = {"test", "13.25", "-d"};
    runTest(a7, 3,
            "0100000000101010100000000000000000000000000000000000000000000000\n",
            "13.25 to double");
}

void test32_IEEEToDec()
{

    char *a1[] = {"test", "00111111100000000000000000000000"};
    runTest(a1, 2, "1\n", "1.0 from single");

    char *a2[] = {"test", "10111111100000000000000000000000"};
    runTest(a2, 2, "-1\n", "-1.0 from single");

    char *a3[] = {"test", "01000000000000000000000000000000"};
    runTest(a3, 2, "2\n", "2.0 from single");

    char *a4[] = {"test", "00111111000000000000000000000000"};
    runTest(a4, 2, "0.5\n", "0.5 from single");

    char *a5[] = {"test", "01000001010101000000000000000000"};
    runTest(a5, 2, "13.25\n", "13.25 from single");

    char *a6[] = {"test", "11000001010101000000000000000000"};
    runTest(a6, 2, "-13.25\n", "-13.25 from single");
}

void test64_IEEEToDec()
{

    char *a1[] = {"test",
                  "0011111111110000000000000000000000000000000000000000000000000000"};
    runTest(a1, 2, "1\n", "1.0 from double");

    char *a2[] = {"test",
                  "1011111111110000000000000000000000000000000000000000000000000000"};
    runTest(a2, 2, "-1\n", "-1.0 from double");

    char *a3[] = {"test",
                  "0100000000000000000000000000000000000000000000000000000000000000"};
    runTest(a3, 2, "2\n", "2.0 from double");

    char *a4[] = {"test",
                  "0011111111100000000000000000000000000000000000000000000000000000"};
    runTest(a4, 2, "0.5\n", "0.5 from double");

    char *a5[] = {"test",
                  "0100000000101010100000000000000000000000000000000000000000000000"};
    runTest(a5, 2, "13.25\n", "13.25 from double");
}

void testSpecial()
{

    // single to dec

    char *s1[] = {"test", "01111111100000000000000000000000"};
    runTest(s1, 2, "inf\n", "+inf from single");

    char *s2[] = {"test", "11111111100000000000000000000000"};
    runTest(s2, 2, "-inf\n", "-inf from single");

    char *s3[] = {"test", "01111111100000000000000000000001"};
    runTest(s3, 2, "nan\n", "NaN from single");

    char *s4[] = {"test", "00000000000000000000000000000001"};
    runTest(s4, 2, "1.401298e-45\n", "smallest denorm single");

    // double to dec

    char *d1[] = {"test",
                  "0111111111110000000000000000000000000000000000000000000000000000"};
    runTest(d1, 2, "inf\n", "+inf from double");

    char *d2[] = {"test",
                  "1111111111110000000000000000000000000000000000000000000000000000"};
    runTest(d2, 2, "-inf\n", "-inf from double");

    char *d3[] = {"test",
                  "0111111111110000000000000000000000000000000000000000000000000001"};
    runTest(d3, 2, "nan\n", "NaN from double");

    char *d4[] = {"test",
                  "0000000000000000000000000000000000000000000000000000000000000001"};
    runTest(d4, 2, "4.940656458e-324\n", "smallest denorm double");

    // dec to single

    char *s5[] = {"test", "inf"};
    runTest(s5, 2, "01111111100000000000000000000000\n", "+inf to single");

    char *s6[] = {"test", "-inf"};
    runTest(s6, 2, "11111111100000000000000000000000\n", "-inf to single");

    char *s7[] = {"test", "nan"};
    runTest(s7, 2, "01111111100000000000000000000001\n", "NaN to single");

    char *s8[] = {"test", "1.401298e-45"};
    runTest(s8, 2, "00000000000000000000000000000001\n", "smallest denorm to single");

    // dec to double

    char *d5[] = {"test", "inf"};
    runTest(d5, 2,
            "0111111111110000000000000000000000000000000000000000000000000000\n",
            "+inf to double");

    char *d6[] = {"test", "-inf"};
    runTest(d6, 2,
            "1111111111110000000000000000000000000000000000000000000000000000\n",
            "-inf to double");

    char *d7[] = {"test", "nan"};
    runTest(d7, 2,
            "0111111111110000000000000000000000000000000000000000000000000001\n",
            "NaN to double");

    char *d8[] = {"test", "4.940656e-324"};
    runTest(d8, 2,
            "0000000000000000000000000000000000000000000000000000000000000001\n",
            "smallest denorm to double");
}

int main()
{

    test32_DecToIEEE();
    test64_DecToIEEE();

    test32_IEEEToDec();
    test64_IEEEToDec();

    testSpecial();

    printf("All tests finished.\n");
    return 0;
}
