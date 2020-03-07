#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef uint32_t ECHAR;
typedef unsigned char CHAR;

#define c(s) ((char*)s)

#define C(s) ((CHAR*)s)

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

long parse_number(CHAR* s, int* sign);

#define OUTPUT_COLOR "26"
#define VERSION_COLOR "92"
#define TRACE_COLOR "1"
#define DIAGNOSTIC_COLOR "9"

#endif
