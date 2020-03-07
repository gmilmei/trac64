#ifndef BOOLEAN_H
#define BOOLEAN_H

#include "string_buf.h"

int parse_boolean(CHAR* s, string_buf* bool);

void unparse_boolean(string_buf* bool);

void boolean_intersection(string_buf* b1, string_buf* b2);

void boolean_union(string_buf* b1, string_buf* b2);

void boolean_complement(string_buf* b1);

void boolean_shift(string_buf* b1, long n, int rotate);

void boolean_set_digit_base(int base);

#endif

