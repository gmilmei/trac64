#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ansi.h"
#include "io.h"
#include "primitives.h"
#include "help.h"

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

/*
 * Primitives for input and output control (PS, RS, CM, RC).
 */

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

static int prim_cm(TRAC* trac, ARGS* args)
{
    CHAR* s = get_arg(args, 1);
    if (strlen(c(s)) > 0) {
        CHAR meta = s[0];
        if (meta != '#' && meta != '(' && meta != ',') {
            trac->meta_char = meta;
        }
    }
    return 0;
}

static int prim_rc(TRAC* trac, ARGS* args)
{
    CHAR c = io_char(trac->fd_in);
    value(args->to, trac, &c, 1);
    return 0;
}

/*
 * Primitives for string storage and deletion (DS, DD, DA).
 */

static int prim_ds(TRAC* trac, ARGS* args)
{
    CHAR* name = get_arg(args, 1);
    CHAR* s = get_arg(args, 2);
    form_define(trac->forms, name, s, strlen(c(s)));
    return 0;
}

static int prim_dd(TRAC* trac, ARGS* args)
{
    for (int i = 1; i < args->n; i++) {
        CHAR* name = get_arg(args, i);
        form_delete(trac->forms, name);
    }
    return 0;
}

static int prim_da(TRAC* trac, ARGS* args)
{
    forms_clear(trac->forms);               
    return 0;
}

/*
 * Primitive for the creation of text macros (SS).
 */

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

/*
 * Call primitive (CL).
 */

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

/*
 * Partial call primitives (CR, CC, CS, CN, IN).
 */

static int prim_cr(TRAC* trac, ARGS* args)
{
    if (args->n > 1) {
        CHAR* name = get_arg(args, 1);
        form* f = form_lookup(trac->forms, name);
        if (f) f->ptr = 0;
    }
    return 0;
}

static int prim_cc(TRAC* trac, ARGS* args)
{
    if (args->n > 1) {
        CHAR* name = get_arg(args, 1);
        CHAR* z = get_arg(args, 2);
        form* f = form_lookup(trac->forms, name);
        if (f) {
            CHAR c;
            int end = form_cc(f, &c);
            if (end)
                value(args->to, trac, z, strlen(c(z)));
            else
                value(args->to, trac, &c, 1);
        }
    }
    return 0;
}

static int prim_cs(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

static int prim_cn(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

static int prim_in(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

/*
 * Arithmetic Primitives (AD, SU, ML, DV).
 */

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
        // TODO
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

/*
 * Boolean primitives (BU, BI, BC, BR, BS).
 */

static int prim_bu(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

static int prim_bi(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

static int prim_bc(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

static int prim_br(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

static int prim_bs(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

/*
 * Decision primitives (EQ, GR).
 */

static int prim_eq(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

static int prim_gr(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

/*
 * Auxiliary storage primitives (SB, FB, EB).
 */

static int prim_sb(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

static int prim_fb(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

static int prim_eb(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

/*
 * Diagnostic primitives (LN, PF, TN, TF).
 */

static int prim_ln(TRAC* trac, ARGS* args)
{
    CHAR* p = get_arg(args, 1);
    form** fs = trac->forms->table;
    int size = trac->forms->size;
    string_buf* sbuf = string_buf_new(256);
    for (int i = 0; i < size; i++) {
        form* f = fs[i];
        string_buf_append(sbuf, p);
        string_buf_append(sbuf, f->name);
    }
    value(args->to, trac, sbuf->buf, sbuf->len);
    return 0;
}

static int prim_pf(TRAC* trac, ARGS* args)
{
    CHAR* name = get_arg(args, 1);
    form* f = form_lookup(trac->forms, name);
    if (f) {
        ECHAR* s = f->buf;
        int i;
        for (i = 0; i < f->len; i++) {
            if (i == f->ptr) {
                fprintf(stdout, "<^>");
            }
            ECHAR ec = s[i];
            if (echar_is_gap(ec)) {
                fprintf(stdout, "<%d>", echar_get_number(ec));
            }
            else {
                CHAR c = ec_to_c(ec);
                fprintf(stdout, "%c", c);
            }
        }
        if (f->ptr == f->len) {
            fprintf(stdout, "<^>");
        }
        fflush(stdout);
    }
    return 0;
}

static int prim_tn(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

static int prim_tf(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

/*
 * Housekeeping primitives: HL, MO
 */

static int prim_hl(TRAC* trac, ARGS* args)
{
    exit(0);
}

static int prim_mo(TRAC* trac, ARGS* args)
{
    // TODO
    return 0;
}

/*
 * Default Call.
 */
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

/*
 * Extensions.
 */

static int prim_help(TRAC* trac, ARGS* args)
{
    write(trac->fd_out, "\n", 1);
    write(trac->fd_out, help_txt, help_txt_len);
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

void primitives_init(int ext)
{
    prims = calloc(MAX_PRIM, sizeof(primitive));
    if (ext) reg_prim("??", prim_help);
    reg_prim("AD", prim_ad);
    reg_prim("BC", prim_bc);
    reg_prim("BI", prim_bi);
    reg_prim("BR", prim_br);
    reg_prim("BS", prim_bs);
    reg_prim("BU", prim_bu);
    reg_prim("CC", prim_cc);
    reg_prim("CL", prim_cl);
    reg_prim("CM", prim_cm);
    reg_prim("CN", prim_cn);
    reg_prim("CR", prim_cr);
    reg_prim("CS", prim_cs);
    reg_prim("DA", prim_da);
    reg_prim("DD", prim_dd);
    reg_prim("DS", prim_ds);
    reg_prim("DV", prim_dv);
    reg_prim("EB", prim_eb);
    reg_prim("EQ", prim_eq);
    reg_prim("FB", prim_fb);
    reg_prim("GR", prim_gr);
    reg_prim("HL", prim_hl);
    reg_prim("IN", prim_in);
    reg_prim("LN", prim_ln);
    reg_prim("ML", prim_ml);
    reg_prim("MO", prim_mo);
    reg_prim("PF", prim_pf);
    reg_prim("PS", prim_ps);
    reg_prim("PS", prim_ps);
    reg_prim("RC", prim_rc);
    reg_prim("RS", prim_rs);
    reg_prim("SB", prim_sb);
    reg_prim("SS", prim_ss);
    reg_prim("SU", prim_su);
    reg_prim("TF", prim_tf);
    reg_prim("TN", prim_tn);
    default_prim.name = C("");
    default_prim.fun = prim_default;
}
