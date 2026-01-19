#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define FRAC_SINGLE 23
#define EXP_SINGLE 8
#define BIAS_SINGLE 127

#define FRAC_DOUBLE 52
#define EXP_DOUBLE 11
#define BIAS_DOUBLE 1023

bool isValidDec(char *s)
{
    if (s==NULL || strlen(s) == 0)
    {
        return false;
    }

    int dotCount = 0;
    int digitCount = 0;

    int i = 0;

    // sign is optional
    if (s[0] == '-' || s[0] == '+')
    {
        i = 1;
    }

    for (; s[i] != '\0'; i++)
    {
        if (s[i] == '.')
        {
            dotCount++;
            if (dotCount > 1)
            {
                return false; // more than one dec point->not valid number}
            }
            else if (s[i] >= '0' && s[i] <= '9')
            {
                digitCount++;
            }
            else
            {
                return false; // bad character
            }
        }
        if (digitCount == 0)
        {
            return false;
        }
        return true;
    }
}

bool isValidSingleOrDouble(char *s)
{
    if (strlen(s) != 64 && strlen(s) != 32)
    {
        return false;
    }
    for (int i = 0; s[i] != '\0'; i++)
    {
        if (s[i] != '0' && s[i] != '1')
        {
            return false;
        }
    }

    return true;
}

double toDecimal(char *s)
{
    double quo = 0.0;
    double frac = 0.0;
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
        {
            dec = true;
        }
        else if (s[i] >= '0' && s[i] <= '9')
        {
            int digit = s[i] - '0';
            if (!dec)
            {
                quo = quo * 10 + digit;
            }
            else
            {
                frac = frac + (digit / divisor);
                divisor = divisor * 10;
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
        {
            value = value | 1;
        }
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
        {
            value = value | 1;
        }
    }
    return value;
}

uint32_t decToSingle(double value)
{
    if (value == 0.0) {
        return 0;
    }
    int sign = 0;
    if (value < 0) {
        sign = 1;
        value = -value;
    }

    int exp;
    double norm = frexp(value, &exp);

    uint32_t mantissa;
    mantissa = (uint32_t)((norm * 2 - 1) * (1 << FRAC_SINGLE));

    int biasedExp;
    biasedExp = exp - 1 + BIAS_SINGLE;

    uint32_t result;

    result = (sign << 31) |
             (biasedExp << FRAC_SINGLE) |
             mantissa;

    return result;
}

uint64_t decToDouble(double value)
{
}

double SingleToDec(uint32_t value)
{
}

double DoubleToDec(uint64_t bits)
{
}

void printSingle(uint32_t value)
{
    for (int i = 31; i >= 0; i--) {
        int bit = (value >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

void printDouble(uint64_t value)
{
    for (int i = 63; i >= 0; i--) {
        int bit = (value >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "No input provided.\n");
        return 1;
    }
    else if (argc == 2)
    {
        if(!isValidSingleOrDouble(argv[1])){
            fprintf(stderr, "Invalid single or double precision format.\n");
            return 1;
        }
        int length = strlen(argv[1]);
        if (length == 32)
        {
            
        }
        else if (length == 64)
        {
        }
        else
        {
            fprintf(stderr, "Input must be 32 or 64 bits.\n");
        }
    }
    else if (argc == 3)
    {
        if (!isValidDec(argv[1])) { 
            fprintf(stderr, "Invalid decimal format.\n"); 
            return 1; 
        }

        double val = toDecimal(argv[1]);
        if (strcmp(argv[3], "-s") == 0)
        {
            uint32_t single = decToSingle(val);
            printSingle(single);
        }
        else if (strcmp(argv[3], "-d") == 0)
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
