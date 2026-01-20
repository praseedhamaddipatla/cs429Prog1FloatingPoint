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
                printf("    expected: %s", expected);
                printf("    actual: %s", result);
        }
}

void test32_DecToIEEE()
{
        char *a1[] = {"test", "-s", "0.0"};
        runTest(a1, 3, "00000000000000000000000000000000\n", "+0 to single");

        char *a2[] = {"test", "-s", "-0.0"};
        runTest(a2, 3, "10000000000000000000000000000000\n", "-0 to single");

        char *a3[] = {"test", "-s", "1.0"};
        runTest(a3, 3, "00111111100000000000000000000000\n", "1.0 to single");

        char *a4[] = {"test", "-s", "-1.0"};
        runTest(a4, 3, "10111111100000000000000000000000\n", "-1.0 to single");

        char *a5[] = {"test", "-s", "2.0"};
        runTest(a5, 3, "01000000000000000000000000000000\n", "2.0 to single");

        char *a6[] = {"test", "-s", "0.5"};
        runTest(a6, 3, "00111111000000000000000000000000\n", "0.5 to single");

        char *a7[] = {"test", "-s", "13.25"};
        runTest(a7, 3, "01000001010101000000000000000000\n", "13.25 to single");

        char *a8[] = {"test", "-s", "-13.25"};
        runTest(a8, 3, "11000001010101000000000000000000\n", "-13.25 to single");
}

void test64_DecToIEEE()
{
        char *a1[] = {"test", "-d", "0.0"};
        runTest(a1, 3,
                "0000000000000000000000000000000000000000000000000000000000000000\n",
                "+0 to double");

        char *a2[] = {"test", "-d", "-0.0"};
        runTest(a2, 3,
                "1000000000000000000000000000000000000000000000000000000000000000\n",
                "-0 to double");

        char *a3[] = {"test", "-d", "1.0"};
        runTest(a3, 3,
                "0011111111110000000000000000000000000000000000000000000000000000\n",
                "1.0 to double");

        char *a4[] = {"test", "-d", "-1.0"};
        runTest(a4, 3,
                "1011111111110000000000000000000000000000000000000000000000000000\n",
                "-1.0 to double");

        char *a5[] = {"test", "-d", "2.0"};
        runTest(a5, 3,
                "0100000000000000000000000000000000000000000000000000000000000000\n",
                "2.0 to double");

        char *a6[] = {"test", "-d", "0.5"};
        runTest(a6, 3,
                "0011111111100000000000000000000000000000000000000000000000000000\n",
                "0.5 to double");

        char *a7[] = {"test", "-d", "13.25"};
        runTest(a7, 3,
                "0100000000101010100000000000000000000000000000000000000000000000\n",
                "13.25 to double");
}

void test32_IEEEToDec()
{
        char *a1[] = {"test", "00111111100000000000000000000000"};
        runTest(a1, 2, "1.0\n", "1.0 from single");  // CORRECTED: was "1\n"

        char *a2[] = {"test", "10111111100000000000000000000000"};
        runTest(a2, 2, "-1.0\n", "-1.0 from single");  // CORRECTED: was "-1\n"

        char *a3[] = {"test", "01000000000000000000000000000000"};
        runTest(a3, 2, "2.0\n", "2.0 from single");  // CORRECTED: was "2\n"

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
        runTest(a1, 2, "1.0\n", "1.0 from double");  // CORRECTED: was "1\n"

        char *a2[] = {"test",
                      "1011111111110000000000000000000000000000000000000000000000000000"};
        runTest(a2, 2, "-1.0\n", "-1.0 from double");  // CORRECTED: was "-1\n"

        char *a3[] = {"test",
                      "0100000000000000000000000000000000000000000000000000000000000000"};
        runTest(a3, 2, "2.0\n", "2.0 from double");  // CORRECTED: was "2\n"

        char *a4[] = {"test",
                      "0011111111100000000000000000000000000000000000000000000000000000"};
        runTest(a4, 2, "0.5\n", "0.5 from double");

        char *a5[] = {"test",
                      "0100000000101010100000000000000000000000000000000000000000000000"};
        runTest(a5, 2, "13.25\n", "13.25 from double");
}

void testSpecial()
{
        //single to dec
        char *s1[] = {"test", "01111111100000000000000000000000"};
        runTest(s1, 2, "inf\n", "+inf from single");

        char *s2[] = {"test", "11111111100000000000000000000000"};
        runTest(s2, 2, "-inf\n", "-inf from single");

        char *s3[] = {"test", "01111111100000000000000000000001"};
        runTest(s3, 2, "NaN\n", "NaN from single");

        char *s4[] = {"test", "00000000000000000000000000000001"};
        runTest(s4, 2, "1.40129846e-45\n", "smallest denorm single");  // CORRECTED precision

        //double to dec
        char *d1[] = {"test",
                      "0111111111110000000000000000000000000000000000000000000000000000"};
        runTest(d1, 2, "inf\n", "+inf from double");

        char *d2[] = {"test",
                      "1111111111110000000000000000000000000000000000000000000000000000"};
        runTest(d2, 2, "-inf\n", "-inf from double");

        char *d3[] = {"test",
                      "0111111111110000000000000000000000000000000000000000000000000001"};
        runTest(d3, 2, "NaN\n", "NaN from double");

        char *d4[] = {"test",
                      "0000000000000000000000000000000000000000000000000000000000000001"};
        runTest(d4, 2, "4.9406564584124654e-324\n", "smallest denorm double");  // CORRECTED

        //dec to single
        char *s5[] = {"test", "-s", "inf"};
        runTest(s5, 3, "01111111100000000000000000000000\n", "+inf to single");

        char *s6[] = {"test", "-s", "-inf"};
        runTest(s6, 3, "11111111100000000000000000000000\n", "-inf to single");

        char *s7[] = {"test", "-s", "NaN"};
        runTest(s7, 3, "01111111100000000000000000000001\n", "NaN to single");

        char *s8[] = {"test", "-s", "1.401298e-45"};
        runTest(s8, 3, "00000000000000000000000000000001\n", "smallest denorm to single");

        //dec to double
        char *d5[] = {"test", "-d", "inf"};
        runTest(d5, 3,
                "0111111111110000000000000000000000000000000000000000000000000000\n",
                "+inf to double");

        char *d6[] = {"test", "-d", "-inf"};
        runTest(d6, 3,
                "1111111111110000000000000000000000000000000000000000000000000000\n",
                "-inf to double");

        char *d7[] = {"test", "-d", "NaN"};
        runTest(d7, 3,
                "0111111111110000000000000000000000000000000000000000000000000001\n",
                "NaN to double");

        char *d8[] = {"test", "-d", "4.940656458e-324"};
        runTest(d8, 3,
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