#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef uint32_t ECHAR;
typedef unsigned char CHAR;

CHAR echar_to_char(ECHAR echar);

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

CHAR* toupper_string(CHAR* p);

#endif
