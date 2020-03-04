#ifndef TRAC_H
#define TRAC_H

#include "active_buf.h"
#include "neutral_buf.h"
#include "string_buf.h"
#include "forms.h"

struct TRAC {
    struct active_buf* abuf;
    struct neutral_buf* nbuf;
    struct forms* forms;

    CHAR meta_char;
    CHAR delete_char;
    CHAR dump_char;

    int fd_in;
    int fd_out;
};

#define TO_ACTIVE 0
#define TO_NEUTRAL 1

struct ARGS {
    struct string_buf* buf;
    int pos[1024];
    int n;
    int to;
};

#endif
