#ifndef BLOCKS_H
#define BLOCKS_H

#include "common.h"
#include "forms.h"
#include "string_buf.h"

void gen_filename_from_blockname(CHAR* blockname, string_buf* filename);

int store_block(CHAR* filename, form* formlist[]);

int fetch_block(CHAR* filename, forms* fs);

int erase_block(CHAR* filename);

#endif
