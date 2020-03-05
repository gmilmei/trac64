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
