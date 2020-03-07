#ifndef ANSI_H
#define ANSI_H

#define ANSI_RED         "1"
#define ANSI_GREEN       "2"
#define ANSI_YELLOW      "3"
#define ANSI_BLUE        "4"
#define ANSI_MAGENTA     "5"
#define ANSI_CYAN        "6"
#define ANSI_GRAY        "7"
#define ANSI_BRIGHT_BLUE "12"

void set_ansi(int enabled);

void ansi_fg(int fd, const char* color);

void ansi_reset(int fd);

#endif
