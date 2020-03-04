#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ansi.h"

static int ansi_enabled = 0;

void set_ansi(int enabled)
{
    ansi_enabled = enabled;
}


void ansi_fg(int fd, char* code)
{
    if (!ansi_enabled) return;
    char buf[16];
    buf[0] = 0x1b;
    buf[1] = 0;
    strcat(buf, "[38;5;");
    strcat(buf, code);
    strcat(buf, "m");
    write(fd, buf, strlen(buf));
}

void ansi_reset(int fd)
{
    if (!ansi_enabled) return;
    write(fd, "\x1B" "[39m", 5);
}
