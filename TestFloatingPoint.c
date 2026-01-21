#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// include for whole file giving error, only declare method being used
int conversion(int argc, char *argv[]);

void testHelper(char *args[], int argc, char *expected, char *testName)
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

void testDecToSingle()
{
        char *a1[] = {"test", "-s", "0.0"};
        testHelper(a1, 3, "00000000000000000000000000000000\n", "+0 to single");

        char *a2[] = {"test", "-s", "-0.0"};
        testHelper(a2, 3, "10000000000000000000000000000000\n", "-0 to single");

        char *a3[] = {"test", "-s", "1.0"};
        testHelper(a3, 3, "00111111100000000000000000000000\n", "1.0 to single");

        char *a4[] = {"test", "-s", "-1.0"};
        testHelper(a4, 3, "10111111100000000000000000000000\n", "-1.0 to single");

        char *a5[] = {"test", "-s", "2.0"};
        testHelper(a5, 3, "01000000000000000000000000000000\n", "2.0 to single");

        char *a6[] = {"test", "-s", "0.5"};
        testHelper(a6, 3, "00111111000000000000000000000000\n", "0.5 to single");

        char *a7[] = {"test", "-s", "13.25"};
        testHelper(a7, 3, "01000001010101000000000000000000\n", "13.25 to single");

        char *a8[] = {"test", "-s", "-13.25"};
        testHelper(a8, 3, "11000001010101000000000000000000\n", "-13.25 to single");
}

void testDecToDouble()
{
        char *a1[] = {"test", "-d", "0.0"};
        testHelper(a1, 3,
                "0000000000000000000000000000000000000000000000000000000000000000\n",
                "+0 to double");

        char *a2[] = {"test", "-d", "-0.0"};
        testHelper(a2, 3,
                "1000000000000000000000000000000000000000000000000000000000000000\n",
                "-0 to double");

        char *a3[] = {"test", "-d", "1.0"};
        testHelper(a3, 3,
                "0011111111110000000000000000000000000000000000000000000000000000\n",
                "1.0 to double");

        char *a4[] = {"test", "-d", "-1.0"};
        testHelper(a4, 3,
                "1011111111110000000000000000000000000000000000000000000000000000\n",
                "-1.0 to double");

        char *a5[] = {"test", "-d", "2.0"};
        testHelper(a5, 3,
                "0100000000000000000000000000000000000000000000000000000000000000\n",
                "2.0 to double");

        char *a6[] = {"test", "-d", "0.5"};
        testHelper(a6, 3,
                "0011111111100000000000000000000000000000000000000000000000000000\n",
                "0.5 to double");

        char *a7[] = {"test", "-d", "13.25"};
        testHelper(a7, 3,
                "0100000000101010100000000000000000000000000000000000000000000000\n",
                "13.25 to double");
}

void testSingleToDec()
{
        char *a1[] = {"test", "00111111100000000000000000000000"};
        testHelper(a1, 2, "1.0\n", "1.0 from single");

        char *a2[] = {"test", "10111111100000000000000000000000"};
        testHelper(a2, 2, "-1.0\n", "-1.0 from single");

        char *a3[] = {"test", "01000000000000000000000000000000"};
        testHelper(a3, 2, "2.0\n", "2.0 from single");

        char *a4[] = {"test", "00111111000000000000000000000000"};
        testHelper(a4, 2, "0.5\n", "0.5 from single");

        char *a5[] = {"test", "01000001010101000000000000000000"};
        testHelper(a5, 2, "13.25\n", "13.25 from single");

        char *a6[] = {"test", "11000001010101000000000000000000"};
        testHelper(a6, 2, "-13.25\n", "-13.25 from single");
}

void testDoubleToDec()
{
        char *a1[] = {"test",
                      "0011111111110000000000000000000000000000000000000000000000000000"};
        testHelper(a1, 2, "1.0\n", "1.0 from double");

        char *a2[] = {"test",
                      "1011111111110000000000000000000000000000000000000000000000000000"};
        testHelper(a2, 2, "-1.0\n", "-1.0 from double");

        char *a3[] = {"test",
                      "0100000000000000000000000000000000000000000000000000000000000000"};
        testHelper(a3, 2, "2.0\n", "2.0 from double");

        char *a4[] = {"test",
                      "0011111111100000000000000000000000000000000000000000000000000000"};
        testHelper(a4, 2, "0.5\n", "0.5 from double");

        char *a5[] = {"test",
                      "0100000000101010100000000000000000000000000000000000000000000000"};
        testHelper(a5, 2, "13.25\n", "13.25 from double");
}

void testSpecial()
{
        char *s1[] = {"test", "01111111100000000000000000000000"};
        testHelper(s1, 2, "inf\n", "+inf from single");

        char *s2[] = {"test", "11111111100000000000000000000000"};
        testHelper(s2, 2, "-inf\n", "-inf from single");

        char *s3[] = {"test", "01111111100000000000000000000001"};
        testHelper(s3, 2, "NaN\n", "NaN from single");

        char *s4[] = {"test", "00000000000000000000000000000001"};
        testHelper(s4, 2, "1.40129846e-45\n", "smallest denorm single");

        char *d1[] = {"test",
                      "0111111111110000000000000000000000000000000000000000000000000000"};
        testHelper(d1, 2, "inf\n", "+inf from double");

        char *d2[] = {"test",
                      "1111111111110000000000000000000000000000000000000000000000000000"};
        testHelper(d2, 2, "-inf\n", "-inf from double");

        char *d3[] = {"test",
                      "0111111111110000000000000000000000000000000000000000000000000001"};
        testHelper(d3, 2, "NaN\n", "NaN from double");

        char *d4[] = {"test",
                      "0000000000000000000000000000000000000000000000000000000000000001"};
        testHelper(d4, 2, "4.9406564584124654e-324\n", "smallest denorm double");

        char *s5[] = {"test", "-s", "inf"};
        testHelper(s5, 3, "01111111100000000000000000000000\n", "+inf to single");

        char *s6[] = {"test", "-s", "-inf"};
        testHelper(s6, 3, "11111111100000000000000000000000\n", "-inf to single");

        char *s7[] = {"test", "-s", "NaN"};
        testHelper(s7, 3, "01111111100000000000000000000001\n", "NaN to single");

        char *s8[] = {"test", "-s", "1.401298e-45"};
        testHelper(s8, 3, "00000000000000000000000000000001\n", "smallest denorm to single");

        char *d5[] = {"test", "-d", "inf"};
        testHelper(d5, 3,
                "0111111111110000000000000000000000000000000000000000000000000000\n",
                "+inf to double");

        char *d6[] = {"test", "-d", "-inf"};
        testHelper(d6, 3,
                "1111111111110000000000000000000000000000000000000000000000000000\n",
                "-inf to double");

        char *d7[] = {"test", "-d", "NaN"};
        testHelper(d7, 3,
                "0111111111110000000000000000000000000000000000000000000000000001\n",
                "NaN to double");

        char *d8[] = {"test", "-d", "4.940656458e-324"};
        testHelper(d8, 3,
                "0000000000000000000000000000000000000000000000000000000000000001\n",
                "smallest denorm to double");
}

void testVeryBig()
{

        char *l1[] = {"test", "-s", "1e10"};
        testHelper(l1, 3, "01010000000101010000001011111001\n", "1e10 to single");

        char *l2[] = {"test", "01010000000101010000001011111001"};
        testHelper(l2, 2, "1.0e10\n", "1e10 from single");

        char *l3[] = {"test", "-s", "3.4028235e38"};
        testHelper(l3, 3, "01111111011111111111111111111111\n", "near max single");

        char *l4[] = {"test", "01111111011111111111111111111111"};
        testHelper(l4, 2, "3.40282346e38\n", "max single back to decimal");

        char *l5[] = {"test", "-d", "1e100"};
        testHelper(l5, 3, "0101010010110010010010011010110100100101100101001100001101111101\n", "1e100 to double");

        char *l6[] = {"test", "0101010010110010010010011010110100100101100101001100001101111101"};
        testHelper(l6, 2, "1.0e100\n", "1e100 from double");
}

void testVerySmall()
{
        char *s1[] = {"test", "-s", "1e-10"};
        testHelper(s1, 3, "00101110110110111110011011111111\n", "1e-10 to single");

        char *s2[] = {"test", "00101110110110111110011011111111"};
        testHelper(s2, 2, "1.00000001e-10\n", "1e-10 from single (shows rounding)");

        char *s3[] = {"test", "00000000000000000000000000000010"};
        testHelper(s3, 2, "2.80259692e-45\n", "denorm x2 from single");

        char *s4[] = {"test", "00000000011111111111111111111111"};
        testHelper(s4, 2, "1.17549421e-38\n", "largest denorm single");

        char *s5[] = {"test", "-s", "1e-20"};
        testHelper(s5, 3, "00011110001111001110010100001000\n", "1e-20 to single");

        char *s6[] = {"test", "00011110001111001110010100001000"};
        testHelper(s6, 2, "9.99999968e-21\n", "1e-20 from single (shows rounding)");
}

void testBoundary()
{
        char *b1[] = {"test", "01111111011111111111111111111111"};
        testHelper(b1, 2, "3.40282346e38\n", "max normal single");

        char *b2[] = {"test", "00000000100000000000000000000000"};
        testHelper(b2, 2, "1.17549435e-38\n", "min normal single");

        char *b3[] = {"test",
                      "0111111111101111111111111111111111111111111111111111111111111111"};
        testHelper(b3, 2, "1.7976931348623157e308\n", "max normal double");

        char *b4[] = {"test",
                      "0000000000010000000000000000000000000000000000000000000000000000"};
        testHelper(b4, 2, "2.2250738585072013e-308\n", "min normal double");
}

void testPrecision()
{
        char *p1[] = {"test", "-s", "0.1"};
        testHelper(p1, 3, "00111101110011001100110011001101\n", "0.1 to single");

        char *p2[] = {"test", "00111101110011001100110011001101"};
        testHelper(p2, 2, "0.100000001\n", "0.1 from single (shows FP imprecision)");

        char *p3[] = {"test", "-s", "0.3"};
        testHelper(p3, 3, "00111110100110011001100110011010\n", "0.3 to single");

        char *p4[] = {"test", "00111110100110011001100110011010"};
        testHelper(p4, 2, "0.300000012\n", "0.3 from single");

        char *p5[] = {"test", "-s", "123.456789"};
        testHelper(p5, 3, "01000010111101101110100111100000\n", "9 digit decimal to single");

        char *p6[] = {"test", "01000010111101101110100111100000"};
        testHelper(p6, 2, "123.456787\n", "verify precision single");

        char *p7[] = {"test", "-d", "0.123456789012345"};
        testHelper(p7, 3, "0011111110111111100110101101110100110111010001101111011000101110\n", "15 digit decimal to double");

        char *p8[] = {"test", "0011111110111111100110101101110100110111010001101111011000101110"};
        testHelper(p8, 2, "0.123456789012345\n", "verify precision double");
}

void testNegZero()
{
        char *z1[] = {"test", "00000000000000000000000000000000"};
        testHelper(z1, 2, "+0\n", "+0 from single");

        char *z2[] = {"test", "10000000000000000000000000000000"};
        testHelper(z2, 2, "-0\n", "-0 from single");

        char *z3[] = {"test",
                      "0000000000000000000000000000000000000000000000000000000000000000"};
        testHelper(z3, 2, "+0\n", "+0 from double");

        char *z4[] = {"test",
                      "1000000000000000000000000000000000000000000000000000000000000000"};
        testHelper(z4, 2, "-0\n", "-0 from double");
}

void testNaN()
{
        char *n1[] = {"test", "01111111100000000000000000000001"};
        testHelper(n1, 2, "NaN\n", "NaN pattern 1 single");

        char *n2[] = {"test", "01111111110000000000000000000000"};
        testHelper(n2, 2, "NaN\n", "NaN pattern 2 single");

        char *n3[] = {"test", "11111111111111111111111111111111"};
        testHelper(n3, 2, "NaN\n", "NaN all bits except sign single");

        char *n4[] = {"test",
                      "0111111111110000000000000000000000000000000000000000000000000001"};
        testHelper(n4, 2, "NaN\n", "NaN pattern 1 double");

        char *n5[] = {"test",
                      "1111111111111111111111111111111111111111111111111111111111111111"};
        testHelper(n5, 2, "NaN\n", "NaN all bits double");
}

void testScientific()
{
        char *t1[] = {"test", "-s", "10000000000"};
        testHelper(t1, 3, "01010000000101010000001011111001\n", "1e10 to single");

        char *t2[] = {"test", "01010000000101010000001011111001"};
        testHelper(t2, 2, "1.0e10\n", "verify uses scientific notation");

        char *t3[] = {"test", "-s", "9999999488"};
        testHelper(t3, 3, "01010000000101010000001011111001\n", "near 1e10 single");

        char *t4[] = {"test", "-s", "1e-11"};
        testHelper(t4, 3, "00101101001011111110101111111111\n", "1e-11 to single");

        char *t5[] = {"test", "00101101001011111110101111111111"};
        testHelper(t5, 2, "9.99999996e-12\n", "1e-11 from single (shows rounding)");

        char *t6[] = {"test", "-d", "1e-10"};
        testHelper(t6, 3, "0011110111011011011111001101111111011001110101111011110110111011\n", "1e-10 to double");

        char *t7[] = {"test", "0011110111011011011111001101111111011001110101111011110110111011"};
        testHelper(t7, 2, "1.0e-10\n", "1e-10 from double");
}

void testSigFigs()
{
        char *f1[] = {"test", "01000001010110100000000000000000"};
        testHelper(f1, 2, "13.625\n", "exact representation single");

        char *f2[] = {"test", "01000001010110100110011001100110"};
        testHelper(f2, 2, "13.6499996\n", "9 sig figs single");

        char *f3[] = {"test",
                      "0100000000101010110000000000000000000000000000000000000000000000"};
        testHelper(f3, 2, "13.375\n", "exact 13.375 double");

        char *f4[] = {"test",
                      "0100000000101011000000000000000000000000000000000000000000000000"};
        testHelper(f4, 2, "13.5\n", "exact 13.5 double");
}

void testDenorm()
{
        char *d1[] = {"test", "00000000000000000000000000000001"};
        testHelper(d1, 2, "1.40129846e-45\n", "smallest denorm single");

        char *d2[] = {"test", "00000000000000000000000000001000"};
        testHelper(d2, 2, "1.12103877e-44\n", "denorm x8 single");

        char *d3[] = {"test", "00000000010000000000000000000000"};
        testHelper(d3, 2, "5.87747175e-39\n", "mid denorm single");

        char *d4[] = {"test", "00000000000010000000000000000000"};
        testHelper(d4, 2, "7.34683969e-40\n", "denorm bit 19 single");

        char *d5[] = {"test",
                      "0000000000000000000000000000000000000000000000000000000000000001"};
        testHelper(d5, 2, "4.9406564584124654e-324\n", "smallest denorm double");

        char *d6[] = {"test",
                      "0000000000000000000000000000000000000000000000000000000000010000"};
        testHelper(d6, 2, "7.9050503334599447e-323\n", "denorm x16 double");
}

void testNeg()
{
        char *neg1[] = {"test", "-s", "-0.5"};
        testHelper(neg1, 3, "10111111000000000000000000000000\n", "-0.5 to single");

        char *neg2[] = {"test", "10111111000000000000000000000000"};
        testHelper(neg2, 2, "-0.5\n", "-0.5 from single");

        char *neg3[] = {"test", "-s", "-3.14159265"};
        testHelper(neg3, 3, "11000000010010010000111111011011\n", "-pi to single");

        char *neg4[] = {"test", "11000000010010010000111111011011"};
        testHelper(neg4, 2, "-3.14159274\n", "-pi from single");

        char *neg5[] = {"test", "-d", "-123.456"};
        testHelper(neg5, 3, "1100000001011110110111010010111100011010100111111011111001110111\n", "-123.456 to double");

        char *neg6[] = {"test", "1100000001011110110111010010111100011010100111111011111001110111"};
        testHelper(neg6, 2, "-123.456\n", "-123.456 from double");
}

void testErrors()
{
        printf("Invalid inputs--should print errors:\n");

        char *inv1[] = {"test", "001"};
        int result1 = conversion(2, inv1);
        if (result1 != 0)
        {
                printf("  Short binary string: correctly rejected\n");
        }
        else
        {
                printf("  Short binary string: ERROR: accepted\n");
        }

        char *inv2[] = {"test", "0000000000000000000000000000000x"};
        int result2 = conversion(2, inv2);
        if (result2 != 0)
        {
                printf("  Invalid character: correctly rejected\n");
        }
        else
        {
                printf("  Invalid character: ERROR: accepted\n");
        }

        char *inv3[] = {"test", "-s", "abc"};
        int result3 = conversion(3, inv3);
        if (result3 != 0)
        {
                printf("  Invalid decimal: correctly rejected\n");
        }
        else
        {
                printf("  Invalid decimal: ERROR: accepted\n");
        }

        char *inv4[] = {"test", "-x", "1.0"};
        int result4 = conversion(3, inv4);
        if (result4 != 0)
        {
                printf("  Invalid flag: correctly rejected\n");
        }
        else
        {
                printf("  Invalid flag: ERROR: accepted\n");
        }

        char *inv5[] = {"test"};
        int result5 = conversion(1, inv5);
        if (result5 != 0)
        {
                printf("  Too few arguments: correctly rejected\n");
        }
        else
        {
                printf("  Too few arguments: ERROR: accepted\n");
        }

        char *inv6[] = {"test", "-s", "1.0", "extra"};
        int result6 = conversion(4, inv6);
        if (result6 != 0)
        {
                printf("  Too many arguments: correctly rejected\n");
        }
        else
        {
                printf("  Too many arguments: ERROR: accepted\n");
        }
}

void testMoreEdgeCases()
{
        char *e1[] = {"test", "-s", "256.0"};
        testHelper(e1, 3, "01000011100000000000000000000000\n", "256.0 to single");

        char *e2[] = {"test", "01000011100000000000000000000000"};
        testHelper(e2, 2, "256.0\n", "256.0 from single (exact)");

        char *e3[] = {"test", "-s", "0.7"};
        testHelper(e3, 3, "00111111001100110011001100110011\n", "0.7 to single");

        char *e4[] = {"test", "00111111001100110011001100110011"};
        testHelper(e4, 2, "0.699999988\n", "0.7 from single (imprecise)");

        char *e5[] = {"test", "-s", "1.0000001"};
        testHelper(e5, 3, "00111111100000000000000000000001\n", "1.0000001 to single");

        char *e6[] = {"test", "00111111100000000000000000000001"};
        testHelper(e6, 2, "1.00000012\n", "next representable after 1.0 single");
}

int main()
{
        testDecToSingle();
        testDecToDouble();

        testSingleToDec();
        testDoubleToDec();

        testSpecial();

        testVeryBig();
        testVerySmall();
        testBoundary();
        testPrecision();
        testNegZero();
        testNaN();
        testScientific();
        testSigFigs();
        testDenorm();
        testNeg();
        testErrors();

        printf("All tests finished.\n");
        return 0;
}