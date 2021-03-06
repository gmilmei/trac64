#include <stdlib.h>
#include <string.h>
#include "active_buf.h"

active_buf* active_buf_new(int max)
{
    active_buf* abuf = calloc(1, sizeof(active_buf));
    abuf->buf = calloc(max, sizeof(CHAR));
    abuf->max = max;
    abuf->top = -1;
    return abuf;
}

void active_buf_free(active_buf* abuf)
{
    free(abuf->buf);
    free(abuf);
}

void active_buf_clear(active_buf* abuf)
{
    abuf->top = -1;
}

int active_buf_is_empty(active_buf* abuf)
{
    return abuf->top < 0;
}


void active_buf_prepend(active_buf* abuf, CHAR* s, int len)
{
    if (len < 0) len = strlen(c(s));
    for (int i = len-1; i >= 0; i--) {
        active_buf_add(abuf, s[i]);
    }
}

void active_buf_add(active_buf* abuf, CHAR c)
{
    abuf->top++;
    if (abuf->top >= abuf->max) {
        abuf->max *= 2;
        abuf->buf = realloc(abuf->buf, (abuf->max)*sizeof(CHAR));
    }
    abuf->buf[abuf->top] = c;
}

CHAR active_buf_top(active_buf* abuf)
{
    int top = abuf->top;
    if (top >= 0)
        return abuf->buf[top];
    else
        return 0;
}

int active_buf_at(active_buf* abuf, CHAR c)
{
    int top = abuf->top;
    if (top >= 0)
        return abuf->buf[top] == c;
    else
        return 0;
}

int active_buf_at1(active_buf* abuf, CHAR c)
{
    int top = abuf->top;
    if (top >= 1)
        return abuf->buf[top-1] == c;
    else
        return 0;
}

void active_buf_pop(active_buf* abuf, int n)
{
    int top = abuf->top;
    if (top < 0)
        return;
    if (n > top+1) n = top+1;
    abuf->top -= n;
}

void active_buf_print(FILE* f, active_buf* abuf)
{
    for (int i = abuf->top; i >= 0; i--) {
        if (abuf->buf[i] == '\n')
            fprintf(f, "\\n");
        else
            fprintf(f, "%c", abuf->buf[i]);
    }
}
