#ifndef FORM_H
#define FORM_H

#include <stdio.h>
#include "common.h"
#include "string_buf.h"

typedef struct {
    CHAR* name;
    ECHAR* buf;
    int len;
    int ptr;
} form;

typedef struct {
    form** table;
    int max;
    int size;
} forms;

forms* forms_new(int max);

void forms_free(forms* fs);

void forms_clear(forms* fs);

void forms_print(FILE* f, forms* fs);

CHAR* form_get(form* f, CHAR** fargs, int fnargs);

void form_ss(form* f, CHAR** fargs, int fnargs);

int form_cc(form* f, CHAR* c);

int form_cs(form* f, string_buf* sbuf);

int form_cn(form* f, long n, int s, string_buf* sbuf);

int form_in(form* f, CHAR* text, string_buf* sbuf);

void form_delete(forms* fs, const CHAR* name);

form* form_lookup(forms* fs, const CHAR* name);

form* form_new(const CHAR* name, const CHAR* s, int len);

void form_set(forms* fs, form* f);

void form_define(forms* fs, const CHAR* name, const CHAR* s, int len);

void form_print(FILE* file, form* f);

#endif
