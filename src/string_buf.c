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

void string_buf_append_number(string_buf* sbuf, long n)
{
    int p = sbuf->len;

    if (n < 0) {
        string_buf_add(sbuf, '-');
        n = -n;
        p++;
    }

    if (n == 0) {
        string_buf_add(sbuf, '0');
        return;
    }

    while (n != 0) {
        string_buf_add(sbuf, (n % 10)+'0');
        n /= 10;
    }

    int len = sbuf->len-p;
    for (int i = 0; i < len/2; i++) {
        int j = sbuf->len-i-1;
        int k = p+i;
        CHAR c = sbuf->buf[k];
        sbuf->buf[k] = sbuf->buf[j];
        sbuf->buf[j] = c;
    }
}
