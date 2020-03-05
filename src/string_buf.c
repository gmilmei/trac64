#include <stdlib.h>
#include <string.h>
#include "string_buf.h"

string_buf* string_buf_new(int max)
{
    string_buf* sbuf = calloc(1, sizeof(string_buf));
    sbuf->buf = calloc(max+1, sizeof(CHAR));
    sbuf->max = max;
    sbuf->len = 0;
    return sbuf;
}

void string_buf_free(string_buf* sbuf)
{
    free(sbuf->buf);
    free(sbuf);
}

void string_buf_add(string_buf* sbuf, CHAR c)
{
    sbuf->len++;
    if (sbuf->len >= sbuf->max) {
        sbuf->max *= 2;
        sbuf->buf = realloc(sbuf->buf, (sbuf->max+1)*sizeof(CHAR));
    }
    sbuf->buf[sbuf->len-1] = c;
    sbuf->buf[sbuf->len] = 0;
}

void string_buf_append(string_buf* sbuf, CHAR* s)
{
    int sl = strlen(c(s))+1;
    while (sbuf->max < sbuf->len+sl) {
        sbuf->max *= 2;
        sbuf->buf = realloc(sbuf->buf, (sbuf->max+1)*sizeof(CHAR));
    }
    while (*s) {
        sbuf->buf[sbuf->len] = *s;
        s++;
        sbuf->len++;
    }
    sbuf->buf[sbuf->len] = 0;
}
