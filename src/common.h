#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef uint32_t ECHAR;
typedef unsigned char CHAR;

CHAR ec_to_c(ECHAR echar);

int echar_is_gap(ECHAR echar);

ECHAR echar_set_gap(ECHAR echar);

int echar_get_number(ECHAR echar);

ECHAR echar_set_number(ECHAR echar, int number);

int echar_is_nprim(ECHAR echar);

ECHAR echar_set_nprim(ECHAR echar);

int echar_is_aprim(ECHAR echar);

ECHAR echar_set_aprim(ECHAR echar);

int echar_is_arg(ECHAR echar);

ECHAR echar_set_arg(ECHAR echar);

#define c(s) ((char*)s)

#define C(s) ((CHAR*)s)

long parse_number(CHAR* s, int* sign);

#endif
