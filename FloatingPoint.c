#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// constants for single precision
#define FRAC_SINGLE 23  
#define EXP_SINGLE 8 
#define BIAS_SINGLE 127
#define NONSIGN_SINGLE 31 

// constants for double precision
#define FRAC_DOUBLE 52 
#define EXP_DOUBLE 11 
#define BIAS_DOUBLE 1023 
#define NONSIGN_DOUBLE 63 

// checks if a string is a valid decimal number
bool isValidDec(char *s)
{
    // special values check
    if (!strcmp(s, "inf") || !strcmp(s, "-inf") || !strcmp(s, "nan") ||
        !strcmp(s, "NaN") || !strcmp(s, "+0") || !strcmp(s, "-0"))
    {
        return true;
    }

    int dotCount = 0;  // keeps track of decimal points (should only have 1)
    int digits = 0;  
    int i = 0;
    
    if (s[0] == '-' || s[0] == '+')
    {
        i++;
    }

    for (; s[i]; i++)
    {
        if (s[i] == '.')
        {
            dotCount++;
            // more than one decimal point -> invalid
            if (dotCount > 1)
            {
                return false;
            }
        }
        // Check if we hit scientific notation (e or E)
        else if (s[i] == 'e' || s[i] == 'E')
        {
            i++;
            if (s[i] == '+' || s[i] == '-')
            {
                i++;
            }
            if (s[i] < '0' || s[i] > '9')
            {
                return false;
            }
            for (; s[i] >= '0' && s[i] <= '9'; i++)
            {
                // just counting through
            }
            return s[i] == '\0';
        }
        else if (s[i] >= '0' && s[i] <= '9')
        {
            digits++;
        }
        else
        {
            // invalid character -> not a decimal
            return false;
        }
    }
    
    // need at least one digit to be valid
    return digits > 0;
}

// checks if a string is a valid binary representation
bool isValidSingleOrDouble(char *s)
{
    int len = strlen(s);
    
    // length must equal 32 or 64
    if (len != 32 && len != 64)
    {
        return false;
    }
    
    // check proper binary
    for (int i = 0; i < len; i++)
    {
        if (s[i] != '0' && s[i] != '1')
        {
            return false;
        }
    }
    
    return true;
}

// converts string to decimal value
double toDecimal(char *s)
{
    // special cases
    if (!strcmp(s, "inf"))
    {
        return INFINITY;
    }
    if (!strcmp(s, "-inf"))
    {
        return -INFINITY;
    }
    if (!strcmp(s, "nan") || !strcmp(s, "NaN"))
    {
        return NAN;
    }
    if (!strcmp(s, "+0"))
    {
        return 0.0;
    }
    if (!strcmp(s, "-0"))
    {
        return -0.0;
    }

    // regular numbers
    return strtod(s, NULL);
}

// converts 32-character binary string to single precision
uint32_t toSingle(char *s)
{
    uint32_t v = 0;
    
    for (int i = 0; i < 32; i++)
    {
        // shift left then add next bit
        v = v << 1;
        if (s[i] == '1')
        {
            v = v | 1;
        }
    }
    
    return v;
}

// converts 64-character binary string to double precision
uint64_t toDouble(char *s)
{
    uint64_t v = 0;
    
    for (int i = 0; i < 64; i++)
    {
        v = v << 1;
        if (s[i] == '1')
        {
            v = v | 1;
        }
    }
    
    return v;
}

// converts decimal to single precision
uint32_t decToSingle(double v)
{
    // special cases
    //NaN
    if (v != v) 
    {
        return 0x7F800001;
    }
    
    // positive infinity
    if (v == INFINITY)
    {
        return 0x7F800000;
    }
    
    // negative infinity
    if (v == -INFINITY)
    {
        return 0xFF800000;
    }
    
    // positive and negative zero
    if (v == 0.0)
    {
        if (1.0 / v < 0.0)
        {
            return 0x80000000; // negative zero
        }
        else
        {
            return 0x00000000; // positive zero
        }
    }

    // check if negative
    int sign = 0;
    if (v < 0.0)
    {
        sign = 1;
    }
    
    if (sign)
    {
        v = -v;
    }

    // get exponent using base-2 logarithm
    int e = floor(log2(v));
    
    // denormalized small
    if (e < -126)
    {
        double scaled = v * pow(2, 126 + FRAC_SINGLE);
        uint32_t frac = (uint32_t)(scaled + 0.5);  // +0.5 for rounding
        return (sign << 31) | frac;
    }

    double m = v / pow(2, e);
    
    uint32_t frac = (uint32_t)((m - 1.0) * (1 << FRAC_SINGLE) + 0.5);
    
    // check if rounding caused overflow
    if (frac >= (1 << FRAC_SINGLE))
    {
        frac = 0;
        e++;
    }
    return (sign << 31) | ((e + BIAS_SINGLE) << FRAC_SINGLE) | frac;
}

// converts decimal to double precision
uint64_t decToDouble(double v)
{
    //special cases
    // NaN
    if (v != v)
    {
        return 0x7FF0000000000001ULL;
    }
    
    // positive infinity
    if (v == INFINITY)
    {
        return 0x7FF0000000000000ULL;
    }
    
    // negative infinity
    if (v == -INFINITY)
    {
        return 0xFFF0000000000000ULL;
    }
    
    // positive and negative zero
    if (v == 0.0)
    {
        if (1.0 / v < 0.0)
        {
            return 0x8000000000000000ULL; // negative zero
        }
        else
        {
            return 0x0000000000000000ULL; // positive zero
        }
    }

    //check if negative
    int sign = 0;
    if (v < 0.0)
    {
        sign = 1;
    }
    
    if (sign)
    {
        v = -v;
    }

    // break the number into mantissa and exponent
    int e;
    double m = frexp(v, &e);
    e--;   
    m *= 2.0;
    
    // denormalized small
    if (e < -1022)
    {
        long double scaled = (long double)v * powl(2.0L, 1022 + FRAC_DOUBLE);
        uint64_t frac = (uint64_t)(scaled + 0.5L);  // round to nearest
        return ((uint64_t)sign << 63) | frac;
    }

    uint64_t frac = (uint64_t)((m - 1.0) * ((uint64_t)1 << FRAC_DOUBLE) + 0.5);
    
    // check if rounding caused overflow
    if (frac >= ((uint64_t)1 << FRAC_DOUBLE))
    {
        frac = 0;
        e++;
    }
    return ((uint64_t)sign << 63) | ((uint64_t)(e + BIAS_DOUBLE) << FRAC_DOUBLE) | frac;
}

// converts a single precision to decimal
double SingleToDec(uint32_t bits)
{
    // get three parts
    int sign = (bits >> NONSIGN_SINGLE) & 1; 
    int exponent = (bits >> FRAC_SINGLE) & 0xFF; 
    uint32_t fraction = bits & 0x7FFFFF; 
    
    // check if infinity or NaN
    if (exponent == 0xFF)
    {
        if (fraction == 0)
        {
            if (sign)
            {
                return -INFINITY;
            }
            else
            {
                return INFINITY;
            }
        }
        else
        {
            return NAN;
        }
    }
    
    // check if zero or denormalized
    if (exponent == 0)
    {
        if (fraction == 0)
        {
            if (sign)
            {
                return -0.0;
            }
            else
            {
                return 0.0;
            }
        }
        
        double v = fraction * pow(2, 1 - BIAS_SINGLE - FRAC_SINGLE);
        if (sign)
        {
            return -v;
        }
        else
        {
            return v;
        }
    }
    
    // calculate the mantissa
    double mantissa = 1.0 + (double)fraction / (1 << FRAC_SINGLE);
    
    // calculate actual value
    double v = mantissa * pow(2, exponent - BIAS_SINGLE);
    
    if (sign)
    {
        return -v;
    }
    else
    {
        return v;
    }
}

// converts a double precision to decimal
double DoubleToDec(uint64_t bits)
{
    // get three parts
    int sign = (bits >> NONSIGN_DOUBLE) & 1; 
    int exponent = (bits >> FRAC_DOUBLE) & 0x7FF;
    uint64_t fraction = bits & 0xFFFFFFFFFFFFFULL;
    
    // check if infinity or NaN
    if (exponent == 0x7FF)
    {
        if (fraction == 0)
        {
            if (sign)
            {
                return -INFINITY;
            }
            else
            {
                return INFINITY;
            }
        }
        else
        {
            return NAN;
        }
    }
    
    // check if zero or denormalized
    if (exponent == 0)
    {
        if (fraction == 0)
        {
            if (sign)
            {
                return -0.0;
            }
            else
            {
                return 0.0;
            }
        }
        
        // long double for denormalized for better precision
        long double v = (long double)fraction * powl(2.0L, 1 - BIAS_DOUBLE - FRAC_DOUBLE);
        if (sign)
        {
            return -(double)v;
        }
        else
        {
            return (double)v;
        }
    }
    
    // normalized number
    double mantissa = 1.0 + (double)fraction / ((uint64_t)1 << FRAC_DOUBLE);
    double v = mantissa * pow(2, exponent - BIAS_DOUBLE);
    
    if (sign)
    {
        return -v;
    }
    else
    {
        return v;
    }
}

// prints single precision in binary format
void printSingle(uint32_t v)
{
    for (int i = 31; i >= 0; i--)
    {
        // shift right by i
        if ((v >> i) & 1)
        {
            putchar('1');
        }
        else
        {
            putchar('0');
        }
    }
    putchar('\n');
}

// prints double precision in binary format
void printDouble(uint64_t v)
{
    for (int i = 63; i >= 0; i--)
    {
        // shift right by i
        if ((v >> i) & 1)
        {
            putchar('1');
        }
        else
        {
            putchar('0');
        }
    }
    putchar('\n');
}

// helper function to print int wo float formatting
void printInt(long long n)
{
    if (n == 0)
    {
        putchar('0');
        return;
    }
    
    if (n < 0)
    {
        putchar('-');
        n = -n;
    }
    
    // digits added in reverse order
    char buf[32];
    int i = 0;
    while (n > 0)
    {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    
    // print digits
    for (int j = i - 1; j >= 0; j--)
    {
        putchar(buf[j]);
    }
}

void printDecimal(double v, int sig)
{
    // special cases
    // NaN
    if (v != v)
    {
        puts("NaN");
        return;
    }
    
    // positive infinity
    if (v == INFINITY)
    {
        puts("inf");
        return;
    }
    
    // negative infinity
    if (v == -INFINITY)
    {
        puts("-inf");
        return;
    }
    
    // positive or negative zero
    if (v == 0.0)
    {
        if (1.0 / v < 0)
        {
            puts("-0");
        }
        else
        {
            puts("+0");
        }
        return;
    }

    double absv = fabs(v);
    
    // check if scientific notation is needed
    if (absv >= 1e10 || absv <= 1e-10)
    {        
        int sign = (v < 0) ? 1 : 0;
        v = fabs(v);
        
        // get each part
        int exp;
        double mantissa;
        
        // very small numbers
        if (absv < 1e-14)
        {
            double scaled = v;
            exp = 0;
            
            while (scaled < 1.0 && exp > -400)
            {
                scaled *= 10.0;
                exp--;
            }
            
            mantissa = scaled;
        }
        else
        {
            exp = (int)floor(log10(v));
            mantissa = v / pow(10.0, exp);
        }
        
        // buffer for output
        char buf[128];
        int pos = 0;
        
        if (sign) buf[pos++] = '-';
        
        // extract digits
        int digits_printed = 0;
        
        // print integer part
        long long int_part = (long long)mantissa;
        char intbuf[32];
        int intlen = 0;
        if (int_part == 0)
        {
            intbuf[intlen++] = '0';
        }
        else
        {
            long long temp = int_part;
            while (temp > 0)
            {
                intbuf[intlen++] = '0' + (temp % 10);
                temp /= 10;
            }
        }
        // copy digits
        for (int i = intlen - 1; i >= 0; i--)
        {
            buf[pos++] = intbuf[i];
            digits_printed++;
        }
        
        double frac = mantissa - int_part;
        
        // print fractional part
        if (digits_printed < sig)
        {
            buf[pos++] = '.';
            
            for (int i = digits_printed; i < sig; i++)
            {
                frac *= 10.0;
                int digit = (int)frac;
                
                // apply rounding on the last digit
                if (i == sig - 1)
                {
                    double next_frac = (frac - digit) * 10.0;
                    if (next_frac >= 5.0)
                    {
                        digit++;
                        // handle carry
                        if (digit >= 10)
                        {
                            // carry backwards
                            digit = 0;
                            int carry_pos = pos - 1;
                            while (carry_pos >= 0)
                            {
                                if (buf[carry_pos] == '.')
                                {
                                    carry_pos--;
                                    continue;
                                }
                                if (buf[carry_pos] == '9')
                                {
                                    buf[carry_pos] = '0';
                                    carry_pos--;
                                }
                                else
                                {
                                    buf[carry_pos]++;
                                    break;
                                }
                            }
                            if (carry_pos < 0 || (carry_pos == 0 && sign))
                            {
                                int insert_pos = sign ? 1 : 0;
                                for (int j = pos; j > insert_pos; j--)
                                {
                                    buf[j] = buf[j-1];
                                }
                                buf[insert_pos] = '1';
                                pos++;
                                exp++;
                            }
                        }
                    }
                }
                
                buf[pos++] = '0' + digit;
                frac -= digit;
            }
        }
        // exponent part
        buf[pos++] = 'e';
        if (exp >= 0)
        {
            buf[pos++] = '+';
        }
        else
        {
            buf[pos++] = '-';
            exp = -exp;
        }
        
        char expbuf[16];
        int explen = 0;
        if (exp == 0)
        {
            expbuf[explen++] = '0';
        }
        else
        {
            int temp = exp;
            while (temp > 0)
            {
                expbuf[explen++] = '0' + (temp % 10);
                temp /= 10;
            }
        }
        // copy digits
        for (int i = explen - 1; i >= 0; i--)
        {
            buf[pos++] = expbuf[i];
        }
        
        buf[pos] = '\0';
        puts(buf);
    }
    else
    {
        //regular dec formatting
        int sign = 0;
        if (v < 0)
        {
            sign = 1;
            v = -v;
        }
        
        // split value
        double intpart;
        double fracpart = modf(v, &intpart);
        
        // check if fraction is negligible
        if (fabs(fracpart) < 1e-10 || fabs(fracpart - 1.0) < 1e-10)
        {
            if (sign) putchar('-');
            printInt((long long)round(v));
            puts(".0");
            return;
        }
        
        // sig figs
        int exp10 = (int)floor(log10(v));
        
        int decimal_places = sig - exp10 - 1;
        
        if (decimal_places < 0)
        {
            decimal_places = 0;
        }
        if (decimal_places > 20)
        {
            decimal_places = 20;
        }
        
        // handle rounding
        double scale = pow(10.0, decimal_places);
        v = round(v * scale) / scale;
        
        modf(v, &intpart);
        fracpart = v - intpart;
        
        // buffer to remove trailing zeros
        char buf[256];
        int pos = 0;
        
        if (sign) buf[pos++] = '-';
        
        long long intval = (long long)intpart;
        char intbuf[32];
        int intlen = 0;
        if (intval == 0)
        {
            intbuf[intlen++] = '0';
        }
        else
        {
            long long temp = intval;
            while (temp > 0)
            {
                intbuf[intlen++] = '0' + (temp % 10);
                temp /= 10;
            }
        }
        // copy digits
        for (int i = intlen - 1; i >= 0; i--)
        {
            buf[pos++] = intbuf[i];
        }
        
        if (decimal_places > 0 && fabs(fracpart) > 1e-15)
        {
            buf[pos++] = '.';
            
            // print each digit
            for (int i = 0; i < decimal_places; i++)
            {
                fracpart *= 10.0;
                int digit = (int)fracpart;
                buf[pos++] = '0' + digit;
                fracpart -= digit;
            }
        }
        else if (decimal_places > 0)
        {
            buf[pos++] = '.';
            buf[pos++] = '0';
        }
        
        // remove trailing zeros
        while (pos > 0 && buf[pos - 1] == '0')
        {
            pos--;
        }
        
        // keep one zero
        if (pos > 0 && buf[pos - 1] == '.')
        {
            buf[pos++] = '0';
        }
        
        buf[pos] = '\0';
        puts(buf);
    }
}

// main method moved here to call in test file
int conversion(int argc, char *argv[])
{
    //binary to decimal
    if (argc == 2)
    {
        // check validity
        if (!isValidSingleOrDouble(argv[1]))
        {
            fprintf(stderr, "Invalid single or double precision format.\n");
            return 1;
        }

        if (strlen(argv[1]) == 32)
        {
            // convert to decimal with 6 sig figs
            printDecimal(SingleToDec(toSingle(argv[1])), 6);
        }
        else
        {
            // convert to decimal with 12 sig figs
            printDecimal(DoubleToDec(toDouble(argv[1])), 12);
        }

        return 0;
    }

    // decimal to binary
    if (argc == 3)
    {
        // check validity
        if (!isValidDec(argv[2]))
        {
            fprintf(stderr, "Invalid decimal format.\n");
            return 1;
        }

        // string to double
        double v = toDecimal(argv[2]);

        if (!strcmp(argv[1], "-s"))
        {
            // convert to single precision
            printSingle(decToSingle(v));
        }
        else if (!strcmp(argv[1], "-d"))
        {
            // convert to double precision
            printDouble(decToDouble(v));
        }
        else
        {
            // invalid flag
            fprintf(stderr, "Invalid flag.\n");
            return 1;
        }
        return 0;
    }

    // wrong number of arguments
    fprintf(stderr, "Invalid number of arguments.\n");
    return 1;
}

int main(int argc, char *argv[]){
    conversion(argc, argv);
}