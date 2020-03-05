#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "trac.h"

typedef int (*primitive_function)(TRAC* trac, ARGS* args);

typedef struct {
    CHAR* name;
    primitive_function fun;
} primitive;

void primitives_init();

primitive* lookup_primitive(CHAR* name);

#endif
