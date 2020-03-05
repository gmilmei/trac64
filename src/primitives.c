#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ansi.h"
#include "io.h"
#include "primitives.h"

#define MAX_PRIM 100

#define OUTPUT_COLOR "26"

static primitive* prims;
static int prim_count = 0;
static primitive default_prim;

static CHAR* get_arg(ARGS* args, int i)
{
    if (args->n > i)
        return &args->buf->buf[args->pos[i]];
    else
        return C("");
}

static void value(int where, TRAC* trac, CHAR* s, int len)
{
    switch (where) {
    case TO_ACTIVE:
        active_buf_prepend(trac->abuf, s, len);
        break;
    case TO_NEUTRAL:
        for (int i = 0; i < len; i++) {
            neutral_buf_add(trac->nbuf, s[i]);
        }
        break;
    }
}

static CHAR** make_fargs(ARGS* args, int* fnargs, int start)
{
    CHAR** fargs = 0;
    *fnargs = 0;
    if (args->n > start) {
        *fnargs = args->n-start;
        fargs = malloc(sizeof(CHAR*)*(*fnargs));
        for (int i = 0; i < *fnargs; i++) {
            fargs[i] = get_arg(args, i+start);
        }
    }
    return fargs;
}

static CHAR* prim_arith_get_args(TRAC* trac, ARGS* args, long* n1, long *n2) {
    CHAR* a1 = get_arg(args, 1);
    CHAR* a2 = get_arg(args, 2);
    int s1 = 0;
    *n1 = parse_number(a1, &s1);
    if (s1 < 0) *n1 *= -1;
    int s2 = 0;
    *n2 = parse_number(a2, &s2);
    if (s2 < 0) *n2 *= -1;
    return a1;
}

static int prim_ad(TRAC* trac, ARGS* args)
{
    long n1, n2;
    CHAR* p = prim_arith_get_args(trac, args, &n1, &n2);
    long n = n1+n2;
    string_buf* sbuf = string_buf_new(128);
    string_buf_append(sbuf, p);
    string_buf_append_number(sbuf, n);
    value(args->to, trac, sbuf->buf, sbuf->len);
    string_buf_free(sbuf);
    return 0;
}

static int prim_su(TRAC* trac, ARGS* args)
{
    long n1, n2;
    CHAR* p = prim_arith_get_args(trac, args, &n1, &n2);
    long n = n1-n2;
    string_buf* sbuf = string_buf_new(128);
    string_buf_append(sbuf, p);
    string_buf_append_number(sbuf, n);
    value(args->to, trac, sbuf->buf, sbuf->len);
    string_buf_free(sbuf);
    return 0;
}

static int prim_ml(TRAC* trac, ARGS* args)
{
    long n1, n2;
    CHAR* p = prim_arith_get_args(trac, args, &n1, &n2);
    long n = n1*n2;
    string_buf* sbuf = string_buf_new(128);
    string_buf_append(sbuf, p);
    string_buf_append_number(sbuf, n);
    value(args->to, trac, sbuf->buf, sbuf->len);
    string_buf_free(sbuf);
    return 0;
}

static int prim_dv(TRAC* trac, ARGS* args)
{
    long n1, n2;
    CHAR* p = prim_arith_get_args(trac, args, &n1, &n2);
    if (n2 == 0) {
        CHAR* ex = get_arg(args, 3);
        value(args->to, trac, ex, strlen(c(ex)));
        return 0;
    }
    long n = n1/n2;
    string_buf* sbuf = string_buf_new(128);
    string_buf_append(sbuf, p);
    string_buf_append_number(sbuf, n);
    value(args->to, trac, sbuf->buf, sbuf->len);
    string_buf_free(sbuf);
    return 0;
}

static int prim_cl(TRAC* trac, ARGS* args)
{
    CHAR* name = get_arg(args, 1);
    form* f = form_lookup(trac->forms, name);
    if (f) {
        int fnargs = 0;
        CHAR** fargs = make_fargs(args, &fnargs, 2);
        CHAR* s = form_get(f, fargs, fnargs);
        free(fargs);
        value(args->to, trac, s, strlen(c(s)));
        free(s);
    }
    return 0;
}

static int prim_ds(TRAC* trac, ARGS* args)
{
    CHAR* name = get_arg(args, 1);
    CHAR* s = get_arg(args, 2);
    form_define(trac->forms, name, s, strlen(c(s)));
    return 0;
}

static int prim_hl(TRAC* trac, ARGS* args)
{
    exit(0);
}

static int prim_ps(TRAC* trac, ARGS* args)
{
    if (args->n > 1) {
        CHAR* s = &args->buf->buf[args->pos[1]];
        int len = strlen(c(s));
        ansi_fg(trac->fd_out, OUTPUT_COLOR);
        io_out(trac->fd_out, s, len);
        ansi_reset(trac->fd_out);
    }
    return 0;
}

static int prim_rs(TRAC* trac, ARGS* args)
{
    ibuf->len = 0;
    io_in(trac->fd_in, trac->fd_out,
          trac->meta_char, trac->dump_char, trac->delete_char,
          ibuf);
    value(args->to, trac, ibuf->buf, ibuf->len);
    return 0;
}

static int prim_ss(TRAC* trac, ARGS* args)
{
    CHAR* name = get_arg(args, 1);
    form* f = form_lookup(trac->forms, name);
    if (f) {
        int fnargs = 0;
        CHAR** fargs = make_fargs(args, &fnargs, 2);
        form_ss(f, fargs, fnargs);
        free(fargs);
    }
    return 0;
}

static int prim_default(TRAC* trac, ARGS* args)
{
    CHAR* name = get_arg(args, 0);
    form* f = form_lookup(trac->forms, name);
    if (f) {
        int fnargs = 0;
        CHAR** fargs = make_fargs(args, &fnargs, 1);
        CHAR* s = form_get(f, fargs, fnargs);
        free(fargs);
        value(args->to, trac, s, strlen(c(s)));
        free(s);
    }
    return 0;
}

static void reg_prim(const char* name, primitive_function fun)
{
    prims[prim_count].name = C(name);
    prims[prim_count].fun = fun;
    prim_count++;
}

static int prim_compare(const void* name, const void* p)
{
    return strcmp(c(name), c(((primitive*)p)->name));
}

primitive* lookup_primitive(CHAR* name)
{
    primitive* p;
    if (strlen(c(name)) == 2) {
        char n[3];
        n[0] = (char)toupper(name[0]);
        n[1] = (char)toupper(name[1]);
        n[2] = 0;
        p = bsearch(n, prims, prim_count,
                    sizeof(primitive), prim_compare);
        if (!p) p = &default_prim;
    }
    else {
        p = &default_prim;
    }
    return p;
}

void primitives_init()
{
    prims = calloc(MAX_PRIM, sizeof(primitive));
    reg_prim("AD", prim_ad);
    reg_prim("CL", prim_cl);
    reg_prim("DS", prim_ds);
    reg_prim("DV", prim_dv);
    reg_prim("HL", prim_hl);
    reg_prim("ML", prim_ml);
    reg_prim("PS", prim_ps);
    reg_prim("RS", prim_rs);
    reg_prim("SS", prim_ss);
    reg_prim("SU", prim_su);
    default_prim.name = C("");
    default_prim.fun = prim_default;
}
