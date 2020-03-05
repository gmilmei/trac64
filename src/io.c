#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include "io.h"

string_buf* ibuf;

void io_init()
{
    ibuf = string_buf_new(1024);
}


static void raw(int fd, struct termios* saved_term)
{
    struct termios new_term;

    if (tcgetattr(fd, saved_term) == -1) {
        perror("saving tty state");
        exit(1);
    }

    new_term = *saved_term;
    new_term.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|IXON);
    new_term.c_lflag &= ~(ICANON|IEXTEN|ECHO);
    new_term.c_cflag &= ~(CSIZE|PARENB);
    new_term.c_cflag |= CS8;

    if (tcsetattr(fd, TCSANOW, &new_term) == -1) {
        perror("setting tty state to raw");
        exit(1);
    }
}

static void cooked(int fd, struct termios* saved_term)
{
    if (tcsetattr(fd, TCSANOW, saved_term) == -1) {
        perror("setting tty state to cooked");
        exit(1);
    }
}

static int get_char(int fd)
{
    char c;
    int n = read(fd, &c, 1);
    if (n != 1)
        return -1;
    else
        return c;
}

static void put_char(int fd, int c)
{
    char buf[1];
    buf[0] = (char)c;
    write(fd, buf, 1);
}

static int back_char(int fd, string_buf* sbuf)
{
    if (sbuf->len > 0 && sbuf->buf[sbuf->len-1] != '\n') {
        put_char(fd, 0x08);
        put_char(fd, 0x20);
        put_char(fd, 0x08);
        sbuf->len--;
        sbuf->buf[sbuf->len] = 0;
        return 1;
    }
    else {
        return 0;
    }
 }

int io_in(int fd_in, int fd_out,
          CHAR meta_char, CHAR dump_char, CHAR delete_char,
          string_buf* sbuf)
{
    struct termios saved_term;
    raw(fd_in, &saved_term);
    while (1) {
        int c = get_char(fd_in);
        if (c == meta_char) {
            put_char(fd_out, c);
            break;
        }
        else if (dump_char > 0 && c == dump_char) {
            while (back_char(fd_out, sbuf));
        }
        else if (c == 0x08 || (delete_char > 0 && c == delete_char)) {
            back_char(fd_out, sbuf);
        }
        else if (c == 0x1b) {
            get_char(fd_in);
        }
        else {
            put_char(fd_out, c);
            string_buf_add(sbuf, c);
        }
    }
    cooked(fd_in, &saved_term);
    return 0;
}

void io_out(int fd, CHAR* s, int len)
{
    write(fd, s, len);
}

CHAR io_char(int fd)
{
    struct termios saved_term;
    raw(fd, &saved_term);
    int c = get_char(fd);
    cooked(fd, &saved_term);
    return (CHAR)c;
}

CHAR* read_file(char* filename)
{
    CHAR* data = 0;
    FILE* file = fopen(filename, "r");
    if (file) {
        char b[1024];
        string_buf* sbuf = string_buf_new(1024);
        int len;
        while ((len = fread(b, sizeof(char), 1024, file)) > 0) {
            string_buf_append_chars(sbuf, b, len);
        }
        string_buf_add(sbuf, 0);
        data = sbuf->buf;
        sbuf->buf = 0;
        string_buf_free(sbuf);
    }
    return data;
}
