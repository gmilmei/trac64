#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
        fflush(stderr);
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
    trac->trace = 0;
}

static void args_init(ARGS* args)
{
    args->buf = string_buf_new(DEFAULT_MAX_BUF_SIZE);
    args->n = 0;
}

static void trace(TRAC* trac, ARGS* args)
{
    ansi_fg(trac->fd_out, ANSI_RED);
    if (args->to == TO_NEUTRAL)
        fprintf(stdout, "##/");
    else
        fprintf(stdout, "#/");    
    for (int i = 0; i < args->n; i++) {
        if (i > 0) fprintf(stdout, "*");
        fprintf(stdout, "%s", &args->buf->buf[args->pos[i]]);
    }
    fflush(stdout);
    CHAR c = io_char(trac->fd_in);
    if (c != '\n') trac->trace = 0;
    fprintf(stdout, "\n");
    fflush(stdout);
    ansi_reset(trac->fd_out);
}

int run(CHAR* script)
{

    if (!script) script = idle_script;
    
    TRAC trac;
    ARGS args;
    trac_init(&trac);
    args_init(&args);

    active_buf* abuf = trac.abuf;
    neutral_buf* nbuf = trac.nbuf;

    for (;;) {
        active_buf_clear(abuf);
        active_buf_prepend(abuf, script, -1);
        script = idle_script;
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
                        // unbalanced parentheses
                        active_buf_clear(abuf);
                        neutral_buf_clear(nbuf);
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

                    if (trac.trace) trace(&trac, &args);

                    nbuf->top = prim_pos-1;
                    nbuf->last_prim = echar_get_number(nbuf->buf[prim_pos]);
                    primitive* prim = lookup_primitive(args.buf->buf);
                    if (prim) prim->fun(&trac, &args);
                }
                else {
                    // unbalanced parentheses
                    active_buf_clear(abuf);
                    neutral_buf_clear(nbuf);
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
                    // regular character
                    neutral_buf_add(nbuf, c);
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
    int opt;
    int ansi = 0;
    int ext = 0;
    int quiet = 0;
    char* script_file = 0;
    CHAR* script = 0;
    char* alt_idle_script_file = 0;
    
    while ((opt = getopt(argc, argv, "qcei:")) != -1) {
        switch (opt) {
        case 'c':
            ansi = 1;
            break;
        case 'e':
            ext = 1;
            break;
        case 'q':
            quiet = 1;
            break;
        case 'i':
            alt_idle_script_file = strdup(optarg);
            break;
        default:
            break;
        }
    }

    script_file = argv[optind];
    if (script_file) {
        script = read_file(script_file);
        if (!script) {
            perror("trac64");
            exit(1);
        }
    }

    if (alt_idle_script_file) {
        // TODO
    }
    
    io_init();
    primitives_init(ext);
    set_ansi(ansi);

    if (!quiet) {
        ansi_fg(STDOUT_FILENO, "92");
        printf("%s %s\n", "TRAC T-64", VERSION_STRING);
        fflush(stdout);
        ansi_reset(STDOUT_FILENO);
    }

    run(script);

    return 0;
}
