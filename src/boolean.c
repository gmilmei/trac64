#include <string.h>
#include "boolean.h"
#include <stdio.h>

static int boolean_digit_base = 8;
static int boolean_digit_mask = 0x7;
static int boolean_digit_bits = 3;

static int boolean_digit_to_value(CHAR c)
{
    switch (boolean_digit_base) {
    case 2:
        if (c == '0' || c == '1')
            return c-'0';
        break;
    case 8:
        if (c >= '0' && c <= '7')
            return c-'0';
        break;
    case 16:
        if (c >= '0' && c <= '9')
            return c-'0';
        else if (c >= 'a' && c <= 'f')
            return 10+c-'a';
        else if (c >= 'A' && c <= 'F')
            return 10+c-'A';
        break;
    }
    return -1;
}

static CHAR boolean_value_to_digit(int b)
{
    switch (boolean_digit_base) {
    case 2:
        return b+'0';
    case 8:
        return b+'0';
        break;
    case 16:
        if (b < 10)
            return b+'0';
        else
            return (b-10)+'A';
        break;
    }
    return '0';
}

static void boolean_adjust(string_buf* b1, string_buf* b2)
{
    while (b1->len < b2->len) {
        string_buf_add(b1, 0);
    }
    while (b2->len < b1->len) {
        string_buf_add(b2, 0);
    }
}

/*
 * Returns 1 if the Boolean argument is a null string,
 * otherwise returns 0.
 */
int parse_boolean(CHAR* s, string_buf* bool)
{
    int i = strlen(c(s))-1;
    int d;
    while (i >= 0 && (d = boolean_digit_to_value(s[i])) >= 0) {
        string_buf_add(bool, d);
        i--;
    }
    if (bool->len == 0) {
        string_buf_add(bool, 0);
        return 1;
    }
    return 0;
}

void unparse_boolean(string_buf* bool)
{
    int len = bool->len;
    for (int i = 0; i < len/2; i++) {
        CHAR c = bool->buf[i];
        bool->buf[i] = bool->buf[len-i-1];
        bool->buf[len-i-1] = c;
    }
    for (int i = 0; i < len; i++) {
        bool->buf[i] = boolean_value_to_digit(bool->buf[i]);
    }
}

void boolean_intersection(string_buf* b1, string_buf* b2)
{
    boolean_adjust(b1, b2);
    for (int i = 0; i < b1->len; i++) {
        b2->buf[i] = b1->buf[i] & b2->buf[i];
    }
}

void boolean_union(string_buf* b1, string_buf* b2)
{
    boolean_adjust(b1, b2);
    for (int i = 0; i < b1->len; i++) {
        b2->buf[i] = b1->buf[i] | b2->buf[i];
    }
}

void boolean_complement(string_buf* b)
{
    for (int i = 0; i < b->len; i++) {
        b->buf[i] = boolean_digit_mask&(~b->buf[i]);
    }
}

void boolean_shift(string_buf* b, long n, int rotate)
{
    if (n == 0) return;
    if (n < 0) {
        n = -n;
        int len = b->len;
        for (int k = 0; k < n; k++) {
            int c = 0;
            if (rotate) c = (b->buf[0]&0x1)<<boolean_digit_bits;
            int m = 0;
            for (int i = len-1; i >= 0; i--) {
                m = b->buf[i];
                m = m | c;
                c = (m&1)?(boolean_digit_base+1):0;
                m = m >> 1;
                b->buf[i] = m&boolean_digit_mask;
            }
        }
    }
    else {
        int len = b->len;
        for (int k = 0; k < n; k++) {
            int c = 0;
            if (rotate) c = (b->buf[len-1]>>(boolean_digit_bits-1))&0x1;
            int m = 0;
            for (int i = 0; i < len; i++) {
                m = b->buf[i];
                m = (m << 1)|c;
                c = (m > boolean_digit_base-1)?1:0;
                b->buf[i] = m&boolean_digit_mask;
            }
        }
    }
}

void boolean_set_digit_base(int base)
{
    switch (base) {
    case 2:
        boolean_digit_base = 2;
        boolean_digit_mask = 0x1;
        boolean_digit_bits = 1;
        break;
    case 8:
        boolean_digit_base = 8;
        boolean_digit_mask = 0x7;
        boolean_digit_bits = 3;
        break;
    case 16:
        boolean_digit_base = 16;
        boolean_digit_mask = 0xF;
        boolean_digit_bits = 4;
        break;
    }   
}
