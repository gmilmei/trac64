#ifndef FORM_H
#define FORM_H

#include <stdio.h>
#include "common.h"

struct form {
    CHAR* name;
    ECHAR* buf;
    int len;
    int ptr;
};

struct forms {
    struct form** table;
    int max;
    int size;
};

struct forms* forms_new(int max);

void forms_free(struct forms*);

void forms_print(FILE* f, struct forms* forms);

CHAR* form_get(struct form* form, CHAR** fargs, int fnargs);

void form_ss(struct form* form, CHAR** fargs, int fnargs);

void form_delete(struct forms* forms, const CHAR* name);

struct form* form_lookup(struct forms*, const CHAR* name);

void form_define(struct forms*, const CHAR* name, const CHAR* s, int len);

void form_print(FILE* file, struct form* form);

#endif
