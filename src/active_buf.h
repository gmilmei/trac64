#ifndef ACTIVE_BUF_H
#define ACTIVE_BUF_H

#include <stdio.h>
#include "common.h"

typedef struct {
    CHAR* buf;
    int top;
    int max;
} active_buf;

active_buf* active_buf_new(int max);

void active_buf_free(active_buf* abuf);

void active_buf_clear(active_buf* abuf);

void active_buf_prepend(active_buf* abuf, CHAR* s, int len);

void active_buf_add(active_buf* abuf, CHAR c);

void active_buf_print(FILE* file, active_buf* abuf);

#endif
