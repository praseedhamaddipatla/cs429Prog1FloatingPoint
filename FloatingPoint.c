#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define FRAC_SINGLE 23
#define EXP_SINGLE 8
#define BIAS_SINGLE 127
#define NONSIGN_SINGLE 31

#define FRAC_DOUBLE 52
#define EXP_DOUBLE 11
#define BIAS_DOUBLE 1023
#define NONSIGN_DOUBLE 63

bool isValidDec(char *s)
{
    if (!strcmp(s, "inf") || !strcmp(s, "-inf") || !strcmp(s, "nan") ||
        !strcmp(s, "NaN") || !strcmp(s, "+0") || !strcmp(s, "-0"))
        return true;

    int dotCount = 0, digits = 0, i = 0;
    if (s[0] == '-' || s[0] == '+')
        i++;

    for (; s[i]; i++)
    {
        if (s[i] == '.')
        {
            dotCount++;
            if (dotCount > 1)
                return false;
        }
        else if (s[i] == 'e' || s[i] == 'E')
        {
            i++;
            if (s[i] == '+' || s[i] == '-')
                i++;
            if (s[i] < '0' || s[i] > '9')
                return false;
            for (; s[i] >= '0' && s[i] <= '9'; i++)
                ;
            return s[i] == '\0';
        }
        else if (s[i] >= '0' && s[i] <= '9')
        {
            digits++;
        }
        else
        {
            return false;
        }
    }
    return digits > 0;
}

bool isValidSingleOrDouble(char *s)
{
    int len = strlen(s);
    if (len != 32 && len != 64)
        return false;
    for (int i = 0; i < len; i++)
        if (s[i] != '0' && s[i] != '1')
            return false;
    return true;
}

double toDecimal(char *s)
{
    if (!strcmp(s, "inf"))
        return INFINITY;
    if (!strcmp(s, "-inf"))
        return -INFINITY;
    if (!strcmp(s, "nan") || !strcmp(s, "NaN"))
        return NAN;
    if (!strcmp(s, "+0"))
        return 0.0;
    if (!strcmp(s, "-0"))
        return -0.0;

    return strtod(s, NULL);
}

uint32_t toSingle(char *s)
{
    uint32_t v = 0;
    for (int i = 0; i < 32; i++)
        v = (v << 1) | (s[i] == '1');
    return v;
}

uint64_t toDouble(char *s)
{
    uint64_t v = 0;
    for (int i = 0; i < 64; i++)
        v = (v << 1) | (s[i] == '1');
    return v;
}

uint32_t decToSingle(double v)
{
    if (v != v)
        return 0x7F800001; // NaN with fraction = 1
    if (v == INFINITY)
        return 0x7F800000;
    if (v == -INFINITY)
        return 0xFF800000;
    
    // Handle zero (positive and negative)
    if (v == 0.0)
    {
        if (1.0 / v < 0.0)
            return 0x80000000; // -0
        else
            return 0x00000000; // +0
    }

    int sign = (v < 0.0) ? 1 : 0;
    if (sign)
        v = -v;

    int e = floor(log2(v));
    
    // Handle denormalized numbers (exponent would be < -126)
    if (e < -126)
    {
        // Denormalized: value = (-1)^s * 0.fraction * 2^(-126)
        // fraction = value * 2^(126 + 23)
        double scaled = v * pow(2, 126 + FRAC_SINGLE);
        uint32_t frac = (uint32_t)(scaled + 0.5);
        return (sign << 31) | frac;
    }

    // Normalized numbers
    double m = v / pow(2, e);
    uint32_t frac = (uint32_t)((m - 1.0) * (1 << FRAC_SINGLE) + 0.5);
    
    // Handle overflow in rounding
    if (frac >= (1 << FRAC_SINGLE))
    {
        frac = 0;
        e++;
    }
    
    return (sign << 31) | ((e + BIAS_SINGLE) << FRAC_SINGLE) | frac;
}

uint64_t decToDouble(double v)
{
    if (v != v)
        return 0x7FF0000000000001ULL; // NaN with fraction = 1
    if (v == INFINITY)
        return 0x7FF0000000000000ULL;
    if (v == -INFINITY)
        return 0xFFF0000000000000ULL;
    
    // Handle zero (positive and negative)
    if (v == 0.0)
    {
        if (1.0 / v < 0.0)
            return 0x8000000000000000ULL; // -0
        else
            return 0x0000000000000000ULL; // +0
    }

    int sign = (v < 0.0) ? 1 : 0;
    if (sign)
        v = -v;

    // Use frexp to get mantissa and exponent in base 2
    int e;
    double m = frexp(v, &e);
    e--; // frexp returns exponent for range [0.5, 1), we need [1, 2)
    m *= 2.0; // Adjust mantissa accordingly
    
    // Handle denormalized numbers (exponent would be < -1022)
    if (e < -1022)
    {
        // Denormalized: value = (-1)^s * 0.fraction * 2^(-1022)
        // We need: fraction = value / 2^(-1022-52) = value * 2^(1022+52)
        long double scaled = (long double)v * powl(2.0L, 1022 + FRAC_DOUBLE);
        uint64_t frac = (uint64_t)(scaled + 0.5L);
        return ((uint64_t)sign << 63) | frac;
    }

    // Normalized numbers
    uint64_t frac = (uint64_t)((m - 1.0) * ((uint64_t)1 << FRAC_DOUBLE) + 0.5);
    
    // Handle overflow in rounding
    if (frac >= ((uint64_t)1 << FRAC_DOUBLE))
    {
        frac = 0;
        e++;
    }
    
    return ((uint64_t)sign << 63) | ((uint64_t)(e + BIAS_DOUBLE) << FRAC_DOUBLE) | frac;
}

double SingleToDec(uint32_t bits)
{
    int sign = (bits >> NONSIGN_SINGLE) & 1;
    int exponent = (bits >> FRAC_SINGLE) & 0xFF;
    uint32_t fraction = bits & 0x7FFFFF;
    
    if (exponent == 0xFF)
        return fraction == 0 ? (sign ? -INFINITY : INFINITY) : NAN;
    
    if (exponent == 0)
    {
        if (fraction == 0)
            return sign ? -0.0 : 0.0;
        // Denormalized
        double v = fraction * pow(2, 1 - BIAS_SINGLE - FRAC_SINGLE);
        return sign ? -v : v;
    }
    
    // Normalized
    double mantissa = 1.0 + (double)fraction / (1 << FRAC_SINGLE);
    double v = mantissa * pow(2, exponent - BIAS_SINGLE);
    return sign ? -v : v;
}

double DoubleToDec(uint64_t bits)
{
    int sign = (bits >> NONSIGN_DOUBLE) & 1;
    int exponent = (bits >> FRAC_DOUBLE) & 0x7FF;
    uint64_t fraction = bits & 0xFFFFFFFFFFFFFULL;
    
    if (exponent == 0x7FF)
        return fraction == 0 ? (sign ? -INFINITY : INFINITY) : NAN;
    
    if (exponent == 0)
    {
        if (fraction == 0)
            return sign ? -0.0 : 0.0;
        // Denormalized: use long double for better precision
        long double v = (long double)fraction * powl(2.0L, 1 - BIAS_DOUBLE - FRAC_DOUBLE);
        return sign ? -(double)v : (double)v;
    }
    
    // Normalized
    double mantissa = 1.0 + (double)fraction / ((uint64_t)1 << FRAC_DOUBLE);
    double v = mantissa * pow(2, exponent - BIAS_DOUBLE);
    return sign ? -v : v;
}

void printSingle(uint32_t v)
{
    for (int i = 31; i >= 0; i--)
        putchar('0' + ((v >> i) & 1));
    putchar('\n');
}

void printDouble(uint64_t v)
{
    for (int i = 63; i >= 0; i--)
        putchar('0' + ((v >> i) & 1));
    putchar('\n');
}

void printDecimal(double v, int sig)
{
    // Handle special cases
    if (v != v)
    {
        puts("NaN");
        return;
    }
    if (v == INFINITY)
    {
        puts("inf");
        return;
    }
    if (v == -INFINITY)
    {
        puts("-inf");
        return;
    }
    if (v == 0.0)
    {
        puts((1.0 / v < 0) ? "-0" : "+0");
        return;
    }

    double absv = fabs(v);
    
    // Check if we need scientific notation
    if (absv >= 1e10 || absv <= 1e-10)
    {
        // Scientific notation
        int sign = v < 0;
        if (sign)
            v = -v;

        int exp10 = floor(log10(v));
        double m = v / pow(10, exp10);

        char buf[128];
        int idx = 0;

        if (sign)
            buf[idx++] = '-';

        // First digit
        int d = (int)m;
        buf[idx++] = '0' + d;
        buf[idx++] = '.';
        m = (m - d) * 10;

        // Generate remaining digits up to sig-1 (since first digit is before decimal)
        int digits_after_decimal = sig - 1;
        for (int k = 0; k < digits_after_decimal; k++)
        {
            d = (int)m;
            buf[idx++] = '0' + d;
            m = (m - d) * 10;
        }

        // Remove trailing zeros after decimal point
        while (idx > 0 && buf[idx - 1] == '0')
            idx--;
        
        // If we're back at the decimal point, keep one zero
        if (idx > 0 && buf[idx - 1] == '.')
        {
            buf[idx++] = '0';
        }

        buf[idx++] = 'e';
        sprintf(buf + idx, "%d", exp10);
        puts(buf);
    }
    else
    {
        // Regular decimal notation
        int sign = v < 0;
        if (sign)
            v = -v;
        
        // Check if it's a round integer
        double intpart;
        double fracpart = modf(v, &intpart);
        
        // Use a small epsilon for comparison due to floating point precision
        if (fabs(fracpart) < 1e-10 || fabs(fracpart - 1.0) < 1e-10)
        {
            // It's a round integer - output with .0
            if (sign)
                printf("-");
            printf("%.0f.0\n", round(v));
            return;
        }
        
        // Not an integer - format with proper significant figures
        char buf[256];
        
        // Determine number of decimal places needed
        int exp10 = (int)floor(log10(v));
        int decimal_places = sig - exp10 - 1;
        
        if (decimal_places < 0)
            decimal_places = 0;
        if (decimal_places > 20)
            decimal_places = 20;
        
        sprintf(buf, "%.*f", decimal_places, sign ? -v : v);
        
        // Remove trailing zeros
        int len = strlen(buf);
        while (len > 0 && buf[len - 1] == '0')
        {
            buf[len - 1] = '\0';
            len--;
        }
        
        // If we removed all decimal places, add .0
        if (len > 0 && buf[len - 1] == '.')
        {
            buf[len] = '0';
            buf[len + 1] = '\0';
        }
        
        puts(buf);
    }
}

int conversion(int argc, char *argv[])
{
    if (argc == 2)
    {
        if (!isValidSingleOrDouble(argv[1]))
        {
            fprintf(stderr, "Invalid single or double precision format.\n");
            return 1;
        }

        if (strlen(argv[1]) == 32)
            printDecimal(SingleToDec(toSingle(argv[1])), 9);
        else
            printDecimal(DoubleToDec(toDouble(argv[1])), 17);

        return 0;
    }

    if (argc == 3)
    {
        if (!isValidDec(argv[2]))
        {
            fprintf(stderr, "Invalid decimal format.\n");
            return 1;
        }

        double v = toDecimal(argv[2]);

        if (!strcmp(argv[1], "-s"))
            printSingle(decToSingle(v));
        else if (!strcmp(argv[1], "-d"))
            printDouble(decToDouble(v));
        else
        {
            fprintf(stderr, "Invalid flag.\n");
            return 1;
        }
        return 0;
    }

    fprintf(stderr, "Invalid number of arguments.\n");
    return 1;
}