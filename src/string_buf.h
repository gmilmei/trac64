#ifndef STRING_BUF_H
#define STRING_BUF_H

#include "common.h"

struct string_buf {
    CHAR* buf;
    int len;
    int max;
};

struct string_buf* string_buf_new(int max);

void string_buf_free(struct string_buf* sbuf);

void string_buf_add(struct string_buf* sbuf, CHAR c);

void string_buf_append(struct string_buf* sbuf, CHAR* c);

#endif
