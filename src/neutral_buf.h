#ifndef NEUTRAL_BUF_H
#define NEUTRAL_BUF_H

#include "common.h"

struct neutral_buf {

    ECHAR* buf;
    int top;
    int max;
    int pc;
    int last_prim;
    int last_arg;
};

struct neutral_buf* neutral_buf_new();

void neutral_buf_free(struct neutral_buf* nbuf);

int neutral_buf_is_empty(struct neutral_buf* abuf);

int neutral_buf_clear(struct neutral_buf* abuf);

void neutral_buf_add(struct neutral_buf* nbuf, ECHAR c);

void neutral_buf_append(struct neutral_buf* nbuf, ECHAR* c, int len);

ECHAR neutral_buf_top(struct neutral_buf* nbuf);

void neutral_buf_print(FILE* f, struct neutral_buf* nbuf);

#endif
