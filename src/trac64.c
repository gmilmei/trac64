#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "active_buf.h"
#include "ansi.h"
#include "forms.h"
#include "io.h"
#include "neutral_buf.h"
#include "primitives.h"
#include "string_buf.h"
#include "trac.h"
#include "version.h"

#define DEFAULT_MAX_BUF_SIZE 1024

static CHAR* idle_script = C("#(ps,#(rs)(\n))");

static int debug = 0;

static void print_buffers(TRAC* trac)
{
    if (debug) {
        neutral_buf_print(stderr, trac->nbuf);
        fprintf(stderr, "__");
        active_buf_print(stderr, trac->abuf);
        fprintf(stderr, "\n");
    }
}

static void trac_init(TRAC* trac)
{
    trac->abuf = active_buf_new(DEFAULT_MAX_BUF_SIZE);
    trac->nbuf = neutral_buf_new(DEFAULT_MAX_BUF_SIZE);
    trac->forms = forms_new(512);
    trac->meta_char = '\'';
    trac->delete_char = 0;
    trac->dump_char = 0x17;
    trac->fd_in = STDIN_FILENO;
    trac->fd_out = STDOUT_FILENO;
}

static void args_init(ARGS* args)
{
    args->buf = string_buf_new(DEFAULT_MAX_BUF_SIZE);
    args->n = 0;
}

int run()
{
    TRAC trac;
    ARGS args;
    trac_init(&trac);
    args_init(&args);

    active_buf* abuf = trac.abuf;
    neutral_buf* nbuf = trac.nbuf;

    for (;;) {
        active_buf_clear(abuf);
        active_buf_prepend(abuf, idle_script, -1);
        while (abuf->top >= 0) {
            print_buffers(&trac);
            CHAR c = abuf->buf[abuf->top];
            if (nbuf->pc > 0) {
                abuf->top--;
                // in protection
                if (c == ')') {
                    nbuf->pc--;
                    if (nbuf->pc == 0) {
                        // end of protection
                    }
                    else if (nbuf->pc < 0) {
                        // TODO: unbalanced parentheses
                    }
                    else {
                        neutral_buf_add(nbuf, c);
                    }
                    print_buffers(&trac);
                }
                else if (c == '(') {
                    nbuf->pc++;
                    neutral_buf_add(nbuf, c);
                }
                else {
                    neutral_buf_add(nbuf, c);
                }
            }
            else if (c == '(') {
                abuf->top--;
                // start protection
                nbuf->pc++;
            }
            else if (c == ')') {
                if (nbuf->last_prim > 0) {
                    abuf->top--;
                    // execute primitive
                    int prim_pos = nbuf->last_prim-1;
                    // build arguments
                    args.to = echar_is_nprim(nbuf->buf[prim_pos])?TO_NEUTRAL:TO_ACTIVE;
                    args.buf->len = 0;
                    args.n = 1;
                    args.pos[0] = 0;
                    for (int i = prim_pos+1; i <= nbuf->top; i++) {
                        if (echar_is_arg(nbuf->buf[i])) {
                            string_buf_add(args.buf, 0);
                            args.pos[args.n] = args.buf->len;
                            args.n++;
                        }
                        else {
                            CHAR c = ec_to_c(nbuf->buf[i]);
                            string_buf_add(args.buf, c);
                        }
                    }
                    string_buf_add(args.buf, 0);

#if 0
                    fprintf(stderr, "to:%d\n", args.to);
                    fprintf(stderr, "nargs:%d\n", args.n);
                    for (int i = 0; i < args.n; i++) {
                        fprintf(stderr, "%d: |%s|\n", i, &args.buf->buf[args.pos[i]]);
                    }
#endif

                    nbuf->top = prim_pos-1;
                    nbuf->last_prim = echar_get_number(nbuf->buf[prim_pos]);
                    primitive* prim = lookup_primitive(args.buf->buf);
                    if (prim) prim->fun(&trac, &args);
                }
                else {
                    // TODO: unbalanced parentheses
                }
            }
            else if (c == '#') {
                abuf->top--;
                if (abuf->top >= 0) {
                    CHAR c1 = abuf->buf[abuf->top];
                    if (c1 == '(') {
                        // primitive #(
                        abuf->top--;
                        ECHAR ec = echar_set_aprim(0);
                        ec = echar_set_number(ec, nbuf->last_prim);
                        neutral_buf_add(nbuf, ec);
                        nbuf->last_prim = nbuf->top+1;
                    }
                    else if (c1 == '#' && abuf->top >= 1) {
                        if (abuf->buf[abuf->top-1] == '(') {
                            // primitive ##(
                            abuf->top -= 2;
                            ECHAR ec = echar_set_nprim(0);
                            ec = echar_set_number(ec, nbuf->last_prim);
                            neutral_buf_add(nbuf, ec);
                            nbuf->last_prim = nbuf->top+1;
                        }
                        else {
                            abuf->top--;
                            neutral_buf_add(nbuf, c1);
                        }
                    }
                    else {
                        // regular character
                        neutral_buf_add(nbuf, c);
                    }
                }
                else {
                    // TODO
                }
            }
            else if (c == ',') {
                abuf->top--;
                ECHAR ec = echar_set_arg(0);
                neutral_buf_add(nbuf, ec);
            }
            else if (isspace(c)) {
                abuf->top--;
            }
            else {
                // regular character
                abuf->top--;
                neutral_buf_add(nbuf, c);
            }
        }
        print_buffers(&trac);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    io_init();
    primitives_init();
    set_ansi(1);
    ansi_fg(STDOUT_FILENO, "92");
    printf("%s %s\n", "TRAC T-64", VERSION_STRING);
    ansi_reset(STDOUT_FILENO);
    run();
    return 0;
}
