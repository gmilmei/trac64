#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ansi.h"

static int ansi_enabled = 0;

void set_ansi(int enabled)
{
    ansi_enabled = enabled;
}

void ansi_fg(int fd, const char* code)
{
    if (!ansi_enabled) return;
    char buf[16];
    buf[0] = 0x1b;
    buf[1] = 0;
    strcat(buf, "[38;5;");
    strcat(buf, code);
    strcat(buf, "m");
    if (write(fd, buf, strlen(buf)) < 0) perror("ansi_fg");
}

void ansi_reset(int fd)
{
    if (!ansi_enabled) return;
    if (write(fd, "\x1B" "[39m", 5) < 0) perror("ansi_reset");
}
