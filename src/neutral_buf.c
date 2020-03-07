#include <stdio.h>
#include <stdlib.h>
#include "neutral_buf.h"

neutral_buf* neutral_buf_new(int max)
{
    neutral_buf* nbuf = calloc(1, sizeof(neutral_buf));
    nbuf->buf = calloc(max, sizeof(ECHAR));
    nbuf->max = max;
    nbuf->top = -1;
    nbuf->last_prim = 0;
    nbuf->last_arg = 0;
    nbuf->pc = 0;
    return nbuf;
}

void neutral_buf_free(neutral_buf* nbuf)
{
    free(nbuf->buf);
    free(nbuf);
}

void neutral_buf_clear(neutral_buf* nbuf)
{
    nbuf->top = -1;
    nbuf->last_prim = 0;
    nbuf->last_arg = 0;
    nbuf->pc = 0;
}

void neutral_buf_add(neutral_buf* nbuf, ECHAR c)
{
    nbuf->top++;
    if (nbuf->top >= nbuf->max) {
        nbuf->max *= 2;
        nbuf->buf = realloc(nbuf->buf, (nbuf->max)*sizeof(ECHAR));
    }
    nbuf->buf[nbuf->top] = c;
}

void neutral_buf_print(FILE* file, neutral_buf* nbuf)
{
    for (int i = 0; i <= nbuf->top; i++) {
        ECHAR ec = nbuf->buf[i];
        if (echar_is_aprim(ec))
            fprintf(file, "[#%d]", echar_get_number(ec));
        else if (echar_is_nprim(ec))
            fprintf(file, "[##%d]", echar_get_number(ec));
        else if (echar_is_arg(ec))
            fprintf(file, "[%d]", echar_get_number(ec));
        else {
            CHAR c = ec_to_c(ec);
            if (c == '\n')
                fprintf(file, "\\n");
            else
                fprintf(file, "%c", ec_to_c(ec));
        }
    }
}
