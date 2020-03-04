#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ansi.h"
#include "io.h"
#include "primitives.h"

#define MAX_PRIM 80

#define OUTPUT_COLOR "26"

static struct primitive* prim_funs;
static int prim_fun_count = 0;
static struct primitive default_primitive;

static CHAR* get_arg(struct ARGS* args, int i)
{
    if (args->n > i)
        return &args->buf->buf[args->pos[i]];
    else
        return (CHAR*)"";
}

static void value(int where, struct TRAC* trac, CHAR* s, int len)
{
    if (where == TO_ACTIVE) {
        active_buf_prepend(trac->abuf, s, len);
    }
    else if (where == TO_NEUTRAL) {
        for (int i = 0; i < len; i++) {
            neutral_buf_add(trac->nbuf, s[i]);
        }
    }
}

static CHAR** make_fargs(struct ARGS* args, int* fnargs, int start)
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

static int prim_cl(struct TRAC* trac, struct ARGS* args)
{
    CHAR* name = toupper_string(get_arg(args, 1));
    struct form* form = form_lookup(trac->forms, name);
    if (form) {
        int fnargs = 0;
        CHAR** fargs = make_fargs(args, &fnargs, 2);
        CHAR* s = form_get(form, fargs, fnargs);
        free(fargs);
        value(args->to, trac, s, strlen((char*)s));
        free(s);
    }
    return 0;
}

static int prim_ds(struct TRAC* trac, struct ARGS* args)
{
    CHAR* name = toupper_string(get_arg(args, 1));
    CHAR* s = get_arg(args, 2);
    form_define(trac->forms, name, s, strlen((char*)s));
    return 0;
}

static int prim_hl(struct TRAC* trac, struct ARGS* args)
{
    exit(0);
}

static int prim_ps(struct TRAC* trac, struct ARGS* args)
{
    if (args->n > 1) {
        CHAR* s = &args->buf->buf[args->pos[1]];
        int len = strlen((char*)s);
        ansi_fg(trac->fd_out, OUTPUT_COLOR);
        io_out(trac->fd_out, s, len);
        ansi_reset(trac->fd_out);
    }
    return 0;
}

static int prim_rs(struct TRAC* trac, struct ARGS* args)
{
    ibuf->len = 0;
    io_in(trac->fd_in, trac->fd_out,
          trac->meta_char, trac->dump_char, trac->delete_char,
          ibuf);
    value(args->to, trac, ibuf->buf, ibuf->len);
    return 0;
}

static int prim_ss(struct TRAC* trac, struct ARGS* args)
{
    CHAR* name = toupper_string(get_arg(args, 1));
    struct form* form = form_lookup(trac->forms, name);
    if (form) {
        int fnargs = 0;
        CHAR** fargs = make_fargs(args, &fnargs, 2);
        form_ss(form, fargs, fnargs);
        free(fargs);
    }
    return 0;
}

static int prim_default(struct TRAC* trac, struct ARGS* args)
{
    CHAR* name = toupper_string(get_arg(args, 0));
    struct form* form = form_lookup(trac->forms, name);
    if (form) {
        int fnargs = 0;
        CHAR** fargs = make_fargs(args, &fnargs, 1);
        CHAR* s = form_get(form, fargs, fnargs);
        free(fargs);
        value(args->to, trac, s, strlen((char*)s));
        free(s);
    }
    return 0;
}

static void reg_prim_fun(const char* name, primitive_function fun)
{
    prim_funs[prim_fun_count].name = (CHAR*)name;
    prim_funs[prim_fun_count].fun = fun;
    prim_fun_count++;
}

static int prim_compare(const void* name, const void* p)
{
    return strcmp((char*)name, (char*)((struct primitive*)p)->name);
}

struct primitive* lookup_primitive(CHAR* name)
{
    toupper_string(name);
    struct primitive* primitive;
    primitive = bsearch(name, prim_funs, prim_fun_count,
                        sizeof(struct primitive), prim_compare);
    if (!primitive) primitive = &default_primitive;
    return primitive;
}

void primitives_init()
{
    prim_funs = calloc(MAX_PRIM, sizeof(struct primitive));
    reg_prim_fun("CL", prim_cl);
    reg_prim_fun("DS", prim_ds);
    reg_prim_fun("HL", prim_hl);
    reg_prim_fun("PS", prim_ps);
    reg_prim_fun("RS", prim_rs);
    reg_prim_fun("SS", prim_ss);
    default_primitive.name = (CHAR*)"";
    default_primitive.fun = prim_default;
}
