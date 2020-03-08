#include <string.h>
#include <stdlib.h>
#include "forms.h"
#include "string_buf.h"

form* form_new(const CHAR* name, const CHAR* s, int len)
{
    form* f = calloc(1, sizeof(form));
    f->name = C(strdup(c(name)));
    if (s) {
        f->buf = malloc(len*sizeof(ECHAR));
        ECHAR* buf = f->buf;
        for (int i = 0; i < len; i++) {
            buf[i] = s[i];
        }
    }
    f->len = len;
    return f;
}

void form_free(form* f)
{
    free(f->buf);
    free(f->name);
    free(f);
}

/*
 * The string returned must be freed.
 */
CHAR* form_get(form* f, CHAR** fargs, int fnargs)
{
    string_buf* sbuf = string_buf_new(f->len*2);
    for (int i = f->ptr; i < f->len; i++) {
        ECHAR ec = f->buf[i];
        if (echar_is_gap(ec)) {
            int n = echar_get_number(ec);
            if (n < fnargs) string_buf_append(sbuf, fargs[n]);
        }
        else {
            string_buf_add(sbuf, ec_to_c(ec));
        }
    }
    string_buf_add(sbuf, 0);
    CHAR* s = sbuf->buf;
    sbuf->buf = 0;
    string_buf_free(sbuf);
    return s;
}

void form_ss(form* f, CHAR** fargs, int fnargs)
{
    f->ptr = 0;
    ECHAR* new_buf = malloc(f->len*sizeof(ECHAR));
    int j = 0;
    int len = f->len;
    for (int i = 0; i < len; i++) {
        ECHAR ec = f->buf[i];
        int k;
        for (k = 0; k < fnargs; k++) {
            CHAR* arg = fargs[k];
            int l = 0;
            while (arg[l] && i+l < len) {
                if (echar_is_gap(f->buf[i+l])) goto K;
                if (arg[l] != ec_to_c(f->buf[i+l])) goto K;
                l++;
            }
            if (!arg[l]) {
                new_buf[j++] = echar_set_number(echar_set_gap(0), k);
                i += l-1;
                break;
            }
            K:;
        }
        /* no pattern found */
        if (k == fnargs) new_buf[j++] = ec;
    }
    free(f->buf);
    f->buf = new_buf;
}

int form_cc(form* f, CHAR* c)
{
    if (f->ptr == f->len) return 1;
    while (f->ptr < f->len) {
        if (echar_is_gap(f->buf[f->ptr])) {
            f->ptr++;
            continue;
        }
        *c = ec_to_c(f->buf[f->ptr]);
        f->ptr++;
        return 0;
    }
    return 1;
}

int form_cs(form* f, string_buf* sbuf)
{
    if (f->ptr == f->len) return 1;
    int p = f->ptr;
    while (f->ptr < f->len && !echar_is_gap(f->buf[f->ptr])) {
        f->ptr++;
    }
    for (int i = p; i < f->ptr; i++) {
        string_buf_add(sbuf, ec_to_c(f->buf[i]));
    }
    if (f->ptr != f->len) f->ptr++;
    return 0;
}

int form_cn(form* f, long n, int s, string_buf* sbuf)
{
    // TODO: check correct ptr movement
    if (n > 0 && s >= 0) {
        if (f->ptr == f->len) return 1;
        while (f->ptr < f->len && n > 0) {
            ECHAR ec = f->buf[f->ptr];
            if (echar_is_gap(ec)) {
                f->ptr++;
                continue;
            }
            string_buf_add(sbuf, ec_to_c(ec));
            n--;
            f->ptr++;
        }
    }
    else if (n > 0 && s < 0) {
        if (f->ptr == 0) return 1;
        while (f->ptr > 0 && n > 0) {
            ECHAR ec = f->buf[f->ptr-1];
            if (echar_is_gap(ec)) {
                f->ptr--;
                continue;
            }
            string_buf_add(sbuf, ec_to_c(ec));
            n--;
            f->ptr--;
        }
        int len = sbuf->len;
        for (int i = 0; i < len/2; i++) {
            CHAR c = sbuf->buf[i];
            sbuf->buf[i] = sbuf->buf[len-i-1];
            sbuf->buf[len-i-1] = c;
        }
    }
    else if (n == 0 && s >= 0) {
        if (f->ptr == f->len) return 1;
    }
    else if (n == 0 && s < 0) {
        if (f->ptr == 0) return 1;
    }
    return 0;
}
int form_in(form* f, CHAR* text, string_buf* sbuf)
{
    int tlen = strlen(c(text));
    int ptr = f->ptr;
    while (ptr < f->len) {
        int p = ptr;
        int tpos = 0;
        while (p < f->len && tpos < tlen) {
            ECHAR ec = f->buf[p];
            if (echar_is_gap(ec)) break;
            if (ec_to_c(ec) != text[tpos]) break;
            p++;
            tpos++;
        }
        if (tpos == tlen) {
            int i = f->ptr;
            while (i < ptr) {
                ECHAR ec = f->buf[i];
                if (echar_is_gap(ec)) {
                    i++;
                    continue;
                }
                string_buf_add(sbuf, ec_to_c(ec));
                i++;
            }
            f->ptr = p;
            return 0;
        }
        ptr++;
    }
    return 1;
}


void form_print(FILE* file, form* f)
{
    fprintf(file, "%s:", f->name);
    for (int i = 0; i < f->len; i++) {
        ECHAR ec = f->buf[i];
        if (echar_is_gap(ec))
            fprintf(file, "[%d]", echar_get_number(ec));
        else
            fprintf(file, "%c", ec_to_c(ec));
    }
}

forms* forms_new(int max)
{
    forms* fs = malloc(sizeof(forms));
    fs->table = calloc(max, sizeof(form*));
    fs->max = max;
    fs->size = 0;
    return fs;
}

void forms_free(forms* fs)
{
    // TODO: currently not used
}

void forms_clear(forms* fs)
{
    for (int i = 0; i < fs->size; i++) {
        form* f = fs->table[i];
        if (f) form_free(f);
    }
    fs->size = 0;
}

void forms_print(FILE* file, forms* fs)
{
    for (int i = 0; i < fs->size; i++) {
        form_print(file, fs->table[i]);
        fprintf(file, "\n");
    }
}

static int form_compare(const void* a, const void* b)
{
    const form* const *af = a;
    const form* const *bf = b;
    if (*af && !*bf) return -1;
    if (!*af && *bf) return 1;
    if (!*af && !*bf) return 0;
    return strcmp(c((*af)->name), c((*bf)->name));
}

static int name_compare(const void* name, const void* b)
{
    const form* const *bf = b;
    return strcmp(c(name), c((*bf)->name));
}

static void forms_sort(forms* fs)
{
    qsort(fs->table, fs->size, sizeof(form*), form_compare);
}

static void ensure_size(forms* fs, int size)
{
    if (size > fs->size) {
        int s = fs->size;
        fs->max *= 2;
        fs->table = realloc(fs->table, fs->max*sizeof(form*));
        for (int i = s; i < fs->max; i++) {
            fs->table[i] = 0;
        }
    }
}

form* form_lookup(forms* fs, const CHAR* name)
{
    form** f;
    f = bsearch(name, fs->table, fs->size,
                sizeof(form*), name_compare);
    if (f)
        return *f;
    else
        return 0;
}

void form_delete(forms* fs, const CHAR* name)
{
    form** f;
    f = bsearch(name, fs->table, fs->size,
                sizeof(form*), name_compare);
    if (f) {
        form_free(*f);
        *f = 0;
        forms_sort(fs);
        fs->size--;
    }
}

void form_set(forms* fs, form* f)
{
    form_delete(fs, f->name);
    ensure_size(fs, fs->size+1);
    fs->table[fs->size] = f;
    fs->size++;
    forms_sort(fs);
}

void form_define(forms* fs, const CHAR* name, const CHAR* s, int len)
{
    form_set(fs, form_new(name, s, len));
}
