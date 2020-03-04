#ifndef ACTIVE_BUF_H
#define ACTIVE_BUF_H

#include <stdio.h>
#include "common.h"

struct active_buf {

    CHAR* buf;
    int top;
    int max;
};

struct active_buf* active_buf_new(int max);

void active_buf_free(struct active_buf* abuf);

void active_buf_clear(struct active_buf* abuf);

void active_buf_prepend(struct active_buf* abuf, CHAR* s, int len);

void active_buf_add(struct active_buf* abuf, CHAR c);

void active_buf_print(FILE* file, struct active_buf* abuf);

#endif
