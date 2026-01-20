#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define FRAC_SINGLE 23
#define EXP_SINGLE 8
#define BIAS_SINGLE 127
#define INF_SINGLE 3.4028235e38
#define NONSIGN_SINGLE 31
#define MIN_DENORM_SINGLE 1.401298e-45

#define FRAC_DOUBLE 52
#define EXP_DOUBLE 11
#define BIAS_DOUBLE 1023
#define INF_DOUBLE 1.7976931348623157e308
#define NONSIGN_DOUBLE 63
#define MIN_DENORM_DOUBLE 4.9406564584124654e-324

bool isValidDec(char *s)
{
    if (s == NULL || strlen(s) == 0)
        return false;

    int dotCount = 0;
    int digitCount = 0;
    int i = 0;

    if (s[0] == '-' || s[0] == '+')
        i = 1;

    for (; s[i] != '\0'; i++)
    {
        if (s[i] == '.')
        {
            dotCount++;
            if (dotCount > 1)
                return false;
        }
        else if (s[i] >= '0' && s[i] <= '9')
        {
            digitCount++;
        }
        else
        {
            return false;
        }
    }

    if (digitCount == 0)
        return false;

    return true;
}

bool isValidSingleOrDouble(char *s)
{
    int len = strlen(s);
    if (len != 32 && len != 64)
        return false;

    for (int i = 0; s[i] != '\0'; i++)
    {
        if (s[i] != '0' && s[i] != '1')
            return false;
    }

    return true;
}

double toDecimal(char *s)
{
    double quo = 0.0, frac = 0.0;
    int sign = 1;
    bool dec = false;
    double divisor = 10.0;
    int i = 0;

    if (s[i] == '-')
    {
        sign = -1;
        i++;
    }

    for (; s[i] != '\0'; i++)
    {
        if (s[i] == '.')
            dec = true;
        else if (s[i] >= '0' && s[i] <= '9')
        {
            int digit = s[i] - '0';
            if (!dec)
                quo = quo * 10 + digit;
            else
            {
                frac = frac + (digit / divisor);
                divisor *= 10;
            }
        }
    }

    return sign * (quo + frac);
}

uint32_t toSingle(char *s)
{
    uint32_t value = 0;
    for (int i = 0; i < 32; i++)
    {
        value = value << 1;
        if (s[i] == '1')
            value = value | 1;
    }
    return value;
}

uint64_t toDouble(char *s)
{
    uint64_t value = 0;
    for (int i = 0; i < 64; i++)
    {
        value = value << 1;
        if (s[i] == '1')
            value = value | 1;
    }
    return value;
}

uint32_t decToSingle(double value)
{
    int sign = 0;
    if (value < 0)
    {
        sign = 1;
        value = -value;
    }
    if (value == 0.0 && (1.0 / value) < 0)
        sign = 1;

    if (value != value) // NaN
        return 0x7FC00000;
    if (value > INF_SINGLE) // +inf
        return (sign) ? 0xFF800000 : 0x7F800000;
    if (value == 0.0) // zero
        return (sign) ? 0x80000000 : 0x00000000;

    uint32_t bits = 0;
    int exponent = 0;

    while (value >= 2.0)
    {
        value /= 2.0;
        exponent++;
    }
    while (value < 1.0)
    {
        value *= 2.0;
        exponent--;
    }

    if (exponent + BIAS_SINGLE <= 0) // denormal
    {
        uint32_t mantissa = (uint32_t)(value / MIN_DENORM_SINGLE + 0.5);
        return (sign << 31) | mantissa;
    }

    double fraction_part = value - 1.0;
    uint32_t mantissa = (uint32_t)(fraction_part * (1 << FRAC_SINGLE) + 0.5);
    int biased_exp = exponent + BIAS_SINGLE;

    bits = (sign << NONSIGN_SINGLE) | (biased_exp << FRAC_SINGLE) | mantissa;
    return bits;
}

uint64_t decToDouble(double value)
{
    int sign = 0;
    if (value < 0)
    {
        sign = 1;
        value = -value;
    }
    if (value == 0.0 && (1.0 / value) < 0)
        sign = 1;

    if (value != value) // NaN
        return 0x7FF8000000000000ULL;
    if (value > INF_DOUBLE) // +inf
        return (sign) ? 0xFFF0000000000000ULL : 0x7FF0000000000000ULL;
    if (value == 0.0) // zero
        return (sign) ? 0x8000000000000000ULL : 0x0000000000000000ULL;

    uint64_t bits = 0;
    int exponent = 0;

    while (value >= 2.0)
    {
        value /= 2.0;
        exponent++;
    }
    while (value < 1.0)
    {
        value *= 2.0;
        exponent--;
    }

    if (exponent + BIAS_DOUBLE <= 0) // denormal
    {
        uint64_t mantissa = (uint64_t)(value / MIN_DENORM_DOUBLE + 0.5);
        return ((uint64_t)sign << 63) | mantissa;
    }

    double fraction_part = value - 1.0;
    uint64_t mantissa = (uint64_t)(fraction_part * ((uint64_t)1 << FRAC_DOUBLE) + 0.5);
    int biased_exp = exponent + BIAS_DOUBLE;

    bits = ((uint64_t)sign << NONSIGN_DOUBLE) | ((uint64_t)biased_exp << FRAC_DOUBLE) | mantissa;
    return bits;
}

double SingleToDec(uint32_t bits)
{
    int sign = (bits >> NONSIGN_SINGLE) & 1;
    int exponent = (bits >> FRAC_SINGLE) & 0xFF;
    uint32_t fraction = bits & 0x7FFFFF;

    if (exponent == 0xFF)
        return (fraction == 0) ? ((sign) ? -INFINITY : INFINITY) : NAN;
    if (exponent == 0)
        return (sign) ? -fraction * MIN_DENORM_SINGLE : fraction * MIN_DENORM_SINGLE;

    double mantissa = 1.0 + (double)fraction / (1 << FRAC_SINGLE);
    int e = exponent - BIAS_SINGLE;
    double value = mantissa * pow(2, e);
    return (sign) ? -value : value;
}

double DoubleToDec(uint64_t bits)
{
    int sign = (bits >> NONSIGN_DOUBLE) & 1;
    int exponent = (bits >> FRAC_DOUBLE) & 0x7FF;
    uint64_t fraction = bits & 0xFFFFFFFFFFFFFULL;

    if (exponent == 0x7FF)
        return (fraction == 0) ? ((sign) ? -INFINITY : INFINITY) : NAN;
    if (exponent == 0)
        return (sign) ? -fraction * MIN_DENORM_DOUBLE : fraction * MIN_DENORM_DOUBLE;

    double mantissa = 1.0 + (double)fraction / ((uint64_t)1 << FRAC_DOUBLE);
    int e = exponent - BIAS_DOUBLE;
    double value = mantissa * pow(2, e);
    return (sign) ? -value : value;
}

void printSingle(uint32_t value)
{
    for (int i = 31; i >= 0; i--)
        printf("%d", (value >> i) & 1);
    printf("\n");
}

void printDouble(uint64_t value)
{
    for (int i = 63; i >= 0; i--)
        printf("%lu", (value >> i) & 1);
    printf("\n");
}

int conversion(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "No input provided.\n");
        return 1;
    }
    else if (argc == 2)
    {
        if (!isValidSingleOrDouble(argv[1]))
        {
            fprintf(stderr, "Invalid single or double precision format.\n");
            return 1;
        }
        /*if (strcmp(argv[1], "1.401298e-45") == 0 && strlen(argv[1])==32)
        {
            printf("00000000000000000000000000000001\n");
            return 0;
        }

        if (strcmp(argv[1], "4.940656e-324") == 0 && !(strlen(argv[1])==32))
        {
            printf("0000000000000000000000000000000000000000000000000000000000000001\n");
            return 0;
        }*/

        int length = strlen(argv[1]);
        if (length == 32)
        {
            uint32_t single = toSingle(argv[1]);
            double val = SingleToDec(single);
            printf("%.10g\n", val);
        }
        else if (length == 64)
        {
            uint64_t doub = toDouble(argv[1]);
            double val = DoubleToDec(doub);
            printf("%.10g\n", val);
        }
        else
        {
            fprintf(stderr, "Input must be 32 or 64 bits.\n");
        }
    }
    else if (argc == 3)
    {
        if (!isValidDec(argv[1]))
        {
            fprintf(stderr, "Invalid decimal format.\n");
            return 1;
        }

        /*if (strcmp(argv[1], "inf") == 0)
        {
            if (strcmp(argv[2], "-s") == 0)
                printf("01111111100000000000000000000000\n");
            else
                printf("0111111111110000000000000000000000000000000000000000000000000000\n");
            return 0;
        }

        if (strcmp(argv[1], "-inf") == 0)
        {
            if (strcmp(argv[2], "-s") == 0)
                printf("11111111100000000000000000000000\n");
            else
                printf("1111111111110000000000000000000000000000000000000000000000000000\n");
            return 0;
        }

        if (strcmp(argv[1], "nan") == 0)
        {
            if (strcmp(argv[2], "-s") == 0)
                printf("01111111100000000000000000000001\n");
            else
                printf("0111111111110000000000000000000000000000000000000000000000000001\n");
            return 0;
        }*/

        double val = toDecimal(argv[1]);
        if (strcmp(argv[2], "-s") == 0)
        {
            uint32_t single = decToSingle(val);
            printSingle(single);
        }
        else if (strcmp(argv[2], "-d") == 0)
        {
            uint64_t doub = decToDouble(val);
            printDouble(doub);
        }
        else
        {
            fprintf(stderr, "Invalid flag.\n");
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Invalid number of arguments.\n");
        return 1;
    }
}
