#include <string.h>
#include <stdlib.h>
#include "forms.h"
#include "string_buf.h"

struct form* form_new(const CHAR* name, const CHAR* s, int len)
{
    struct form* form = calloc(1, sizeof(struct form));
    form->name = (CHAR*)strdup((char*)name);
    form->buf = malloc(len*sizeof(ECHAR));
    ECHAR* buf = form->buf;
    for (int i = 0; i < len; i++) {
        buf[i] = s[i];
    }
    form->len = len;
    return form;
}

void form_free(struct form* form)
{
    free(form->buf);
    free(form->name);
    free(form);
}

/*
 * The string returned must be freed.
 */
CHAR* form_get(struct form* form, CHAR** fargs, int fnargs)
{
    struct string_buf* sbuf = string_buf_new(form->len*2);
    for (int i = 0; i < form->len; i++) {
        ECHAR ec = form->buf[i];
        if (echar_is_gap(ec)) {
            int n = echar_get_number(ec);
            if (n < fnargs) string_buf_append(sbuf, fargs[n]);
        }
        else {
            string_buf_add(sbuf, echar_to_char(ec));
        }
    }
    string_buf_add(sbuf, 0);
    CHAR* s = sbuf->buf;
    sbuf->buf = 0;
    string_buf_free(sbuf);
    return s;
}

void form_ss(struct form* form, CHAR** fargs, int fnargs)
{
    ECHAR* new_buf = malloc(form->len*sizeof(ECHAR));
    int j = 0;
    int len = form->len;
    for (int i = 0; i < len; i++) {
        ECHAR ec = form->buf[i];
        int k;
        for (k = 0; k < fnargs; k++) {
            CHAR* arg = fargs[k];
            int l = 0;
            while (arg[l] && i+l < len) {
                if (echar_is_gap(form->buf[i+l])) goto K;
                if (arg[l] != echar_to_char(form->buf[i+l])) goto K;
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
    free(form->buf);
    form->buf = new_buf;
}

void form_print(FILE* f, struct form* form)
{
    fprintf(f, "%s:", form->name);
    for (int i = 0; i < form->len; i++) {
        ECHAR ec = form->buf[i];
        if (echar_is_gap(ec))
            fprintf(f, "[%d]", echar_get_number(ec));
        else
            fprintf(f, "%c", echar_to_char(ec));
    }
}

struct forms* forms_new(int max)
{
    struct forms* forms = malloc(sizeof(struct forms));
    forms->table = calloc(max, sizeof(struct form*));
    forms->max = max;
    forms->size = 0;
    return forms;
}

void forms_free(struct forms* forms)
{
    // TODO
}

static int form_compare(const void* a, const void* b)
{
    const struct form* const *af = a;
    const struct form* const *bf = b;
    if (*af && !*bf) return -1;
    if (!*af && *bf) return 1;
    if (!*af && !*bf) return 0;
    return strcmp((char*)(*af)->name, (char*)(*bf)->name);
}

static int name_compare(const void* name, const void* b)
{
    const struct form* const *bf = b;
    return strcmp((char*)name, (char*)(*bf)->name);
}

static void forms_sort(struct forms* forms)
{
    qsort(forms->table, forms->size, sizeof(struct form*), form_compare);
}

static void ensure_size(struct forms* forms, int size)
{
    if (size > forms->size) {
        int s = forms->size;
        forms->max *= 2;
        forms->table = realloc(forms->table, forms->max*sizeof(struct form*));
        for (int i = s; i < forms->max; i++) {
            forms->table[i] = 0;
        }
    }
}

struct form* form_lookup(struct forms* forms, const CHAR* name)
{
    struct form** form;
    form = bsearch(name, forms->table, forms->size,
                   sizeof(struct form*), name_compare);
    if (form)
        return *form;
    else
        return 0;
}

void form_delete(struct forms* forms, const CHAR* name)
{
    struct form** form;
    form = bsearch(name, forms->table, forms->size,
                   sizeof(struct form*), name_compare);
    if (form) {
        form_free(*form);
        *form = 0;
        forms_sort(forms);
        forms->size--;
    }
}

void form_define(struct forms* forms, const CHAR* name, const CHAR* s, int len)
{
    form_delete(forms, name);
    ensure_size(forms, forms->size+1);
    struct form* form = form_new(name, s, len);
    forms->table[forms->size] = form;
    forms->size++;
    forms_sort(forms);
}

void forms_print(FILE* f, struct forms* forms)
{
    for (int i = 0; i < forms->size; i++) {
        form_print(f, forms->table[i]);
        fprintf(f, "\n");
    }
}
