#ifndef NEUTRAL_BUF_H
#define NEUTRAL_BUF_H

#include "common.h"

typedef struct {
    ECHAR* buf;
    int top;
    int max;
    int pc;
    int last_prim;
    int last_arg;
} neutral_buf;

neutral_buf* neutral_buf_new();

void neutral_buf_free(neutral_buf* nbuf);

int neutral_buf_is_empty(neutral_buf* nbuf);

void neutral_buf_clear(neutral_buf* nbuf);

void neutral_buf_add(neutral_buf* nbuf, ECHAR c);

void neutral_buf_append(neutral_buf* nbuf, ECHAR* c, int len);

ECHAR neutral_buf_top(neutral_buf* nbuf);

void neutral_buf_print(FILE* file, neutral_buf* nbuf);

#endif
