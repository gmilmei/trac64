#ifndef IO_H
#define IO_H

#include "string_buf.h"

extern string_buf* ibuf;

void io_init();

int io_in(int fd_in, int fd_out,
          CHAR meta_char, CHAR dump_char, CHAR delete_char,
          string_buf* sbuf);

void io_out(int fd, CHAR* s, int len);

CHAR io_char(int fd);

CHAR* read_file(char* filename);

#endif
