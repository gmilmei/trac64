#include <string.h>
#include <stdlib.h>
#include "forms.h"
#include "string_buf.h"

form* form_new(const CHAR* name, const CHAR* s, int len)
{
    form* f = calloc(1, sizeof(form));
    f->name = C(strdup(c(name)));
    f->buf = malloc(len*sizeof(ECHAR));
    ECHAR* buf = f->buf;
    for (int i = 0; i < len; i++) {
        buf[i] = s[i];
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
            if (i+l < len) {
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
    // TODO
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

void form_define(forms* fs, const CHAR* name, const CHAR* s, int len)
{
    form_delete(fs, name);
    ensure_size(fs, fs->size+1);
    form* f = form_new(name, s, len);
    fs->table[fs->size] = f;
    fs->size++;
    forms_sort(fs);
}
