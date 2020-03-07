#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "forms.h"
#include "blocks.h"

static char header[] = {0x0, 0xE, 0x1, 'T', '6', '4'};

static void fwrite_int(int n, FILE* file)
{
    fwrite(&n, sizeof(int), 1, file);
}

static int fread_int(FILE* file)
{
    int n = 0;
    fread(&n, sizeof(int), 1, file);
    return n;
}

static void fwrite_string(CHAR* s, FILE* file)
{
    while (*s) {
        fwrite(s, sizeof(CHAR), 1, file);
        s++;
    }
    fputc(0, file);
}

static void fread_string(string_buf* sbuf, FILE* file)
{
    while (!feof(file)) {
        int c = fgetc(file);
        if (c == 0) break;
        string_buf_add(sbuf, (CHAR)c);
    }
    string_buf_add(sbuf, 0);
}

static void fwrite_echar(ECHAR ec, FILE* file)
{
    fwrite(&ec, sizeof(ECHAR), 1, file);
}

static ECHAR fread_echar(FILE* file)
{
    ECHAR ec = 0;
    fread(&ec, sizeof(ECHAR), 1, file);
    return ec;
}

void gen_filename_from_blockname(CHAR* blockname, string_buf* filename)
{
    if (!*blockname) {
        string_buf_append(filename, C("%00.blk"));
        string_buf_add(filename, 0);
        return;
    }
    
    while (*blockname) {
        char c = *blockname;
        if ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z')) {
            string_buf_add(filename, c);
        }
        else {
            int h1 = (c>>4)&0x0F;
            int h2 = (c&0xF);
            string_buf_add(filename, '%');
            string_buf_add(filename, (h1<10)?h1+'0':h1-10+'A');
            string_buf_add(filename, (h2<10)?h2+'0':h2-10+'A');
        }
        blockname++;
    }
    string_buf_append(filename, C(".blk"));
    string_buf_add(filename, 0);
}

int store_block(CHAR* filename, form* formlist[])
{
    FILE* file = fopen(c(filename), "w");
    if (!file) return 0;
    fwrite(header, 6, 1, file);
    while (*formlist) {
        form* f = *formlist;
        fwrite_string(f->name, file);
        fwrite_int(f->ptr, file);
        fwrite_int(f->len, file);
        for (int i = 0; i < f->len; i++) {
            fwrite_echar(f->buf[i], file);
        }
        formlist++;
    }
    fclose(file);
    return 1;
}

int fetch_block(CHAR* filename, forms* fs)
{
    FILE* file = fopen(c(filename), "r");
    if (!file) return 0;
    char h[6];
    int r = fread(h, 1, 6, file);
    if (r != 6) {
        fclose(file);
        return 0;
    }
    // check header
    if (memcmp(header, h, 6) != 0) {
        fclose(file);
        return 0;
    }
    string_buf* name = string_buf_new(128);
    
    while (1) {
        // read form name
        name->len = 0;
        fread_string(name, file);
        if (feof(file)) break;

        // read form pointer
        int ptr = fread_int(file);

        // read form length
        int len = fread_int(file);
        
        if (len < 0) {
            string_buf_free(name);
            fclose(file);
            return 0;
        }

        // read form buffer
        ECHAR* buf = malloc(len*sizeof(ECHAR));
        for (int i = 0; i < len; i++) {
            buf[i] = fread_echar(file);
        }

        // install form
        form* f = form_new(name->buf, 0, 0);
        f->len = len;
        f->ptr = ptr;
        f->buf = buf;
        form_set(fs, f);
    }
    
    string_buf_free(name);
    return 1;
}
