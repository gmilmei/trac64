#include <stdio.h>
#include <stdlib.h>
#include "neutral_buf.h"

struct neutral_buf* neutral_buf_new(int max)
{
    struct neutral_buf* nbuf = calloc(1, sizeof(struct neutral_buf));
    nbuf->buf = calloc(max, sizeof(ECHAR));
    nbuf->max = max;
    nbuf->top = -1;
    nbuf->last_prim = 0;
    nbuf->last_arg = 0;
    nbuf->pc = 0;
    return nbuf;
}

void neutral_buf_free(struct neutral_buf* nbuf)
{
    free(nbuf->buf);
    free(nbuf);
}

void neutral_buf_add(struct neutral_buf* nbuf, ECHAR c)
{
    nbuf->top++;
    if (nbuf->top >= nbuf->max) {
        nbuf->max *= 2;
        nbuf->buf = realloc(nbuf->buf, (nbuf->max)*sizeof(ECHAR));
    }
    nbuf->buf[nbuf->top] = c;
}

void neutral_buf_print(FILE* f, struct neutral_buf* nbuf)
{
    for (int i = 0; i <= nbuf->top; i++) {
        ECHAR ec = nbuf->buf[i];
        if (echar_is_aprim(ec))
            fprintf(f, "[#%d]", echar_get_number(ec));
        else if (echar_is_nprim(ec))
            fprintf(f, "[##%d]", echar_get_number(ec));
        else if (echar_is_arg(ec))
            fprintf(f, "[%d]", echar_get_number(ec));
        else {
            CHAR c = echar_to_char(ec);
            if (c == '\n')
                fprintf(f, "\\n");
            else
                fprintf(f, "%c", echar_to_char(ec));
        }
    }
}
