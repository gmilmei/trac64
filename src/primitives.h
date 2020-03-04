#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "trac.h"

typedef int (*primitive_function)(struct TRAC* trac, struct ARGS* args);

struct primitive {
    CHAR* name;
    primitive_function fun;
};

void primitives_init();

struct primitive* lookup_primitive(CHAR* name);

#endif
