#ifndef FORM_H
#define FORM_H

#include <stdio.h>
#include "common.h"

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

void forms_print(FILE* f, forms* forms);

CHAR* form_get(form* f, CHAR** fargs, int fnargs);

void form_ss(form* f, CHAR** fargs, int fnargs);

void form_delete(forms* ffs, const CHAR* name);

form* form_lookup(forms* fs, const CHAR* name);

void form_define(forms* fs, const CHAR* name, const CHAR* s, int len);

void form_print(FILE* file, form* f);

#endif
