#include <string.h>
#include "common.h"

CHAR ec_to_c(ECHAR echar)
{
    return echar&0xFF;
}

int echar_is_gap(ECHAR echar)
{
    return (echar&0xE000) == 0xE000;
}

ECHAR echar_set_gap(ECHAR echar)
{
    return echar|0xE000;
}

int echar_get_number(ECHAR echar)
{
    return echar&0x1FFF;
}

ECHAR echar_set_number(ECHAR echar, int n)
{
    return (echar&0xE000)|(n&0x1FFF);
}

int echar_is_nprim(ECHAR echar)
{
    return (echar&0xE000) == 0xC000;
}

ECHAR echar_set_nprim(ECHAR echar)
{
    return echar|0xC000;
}

int echar_is_aprim(ECHAR echar)
{
    return (echar&0xE000) == 0xE000;
}

ECHAR echar_set_aprim(ECHAR echar)
{
    return echar|0xE000;
}

int echar_is_arg(ECHAR echar)
{
    return (echar&0xE000) == 0x8000;
}

ECHAR echar_set_arg(ECHAR echar)
{
    return echar|0x8000;
}

long parse_number(CHAR* s, int* sign)
{
    long n = 0;
    int len = strlen(c(s));
    *sign = 0;
    if (len == 0) {
        return n;
    }
    int p = len-1;
    while (p >= 0 && s[p] >= '0' && s[p] <= '9') {
        p--;
    }
    if (p >= 0 && (s[p] == '+' || s[p] == '-')) {
        *sign = s[p] == '+'?1:-1;
        p--;
    }

    for (int i = p+1; i < len; i++) {
        if (s[i] >= '0' && s[i] <= '9')
            n = n*10+(s[i]-'0');
    }

    s[p+1] = 0;

    return n;
}
