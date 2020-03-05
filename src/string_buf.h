#ifndef STRING_BUF_H
#define STRING_BUF_H

#include "common.h"

typedef struct {
    CHAR* buf;
    int len;
    int max;
} string_buf;

string_buf* string_buf_new(int max);

void string_buf_free(string_buf* sbuf);

void string_buf_add(string_buf* sbuf, CHAR c);

void string_buf_append(string_buf* sbuf, CHAR* c);

#endif
