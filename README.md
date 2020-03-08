# TRAC T-64 Programming Language

TRAC is a purely text-based and interactive language, also called
"reactive typewriter" by its inventor Calvin Mooers. It was developed
between 1959 and 1964 and implemented on a PDP-10. It influenced many
later macro processors, such as M4.

For more information, official definition and handbook, see 
[TRAC foundation](https://web.archive.org/web/20041011175826/http://www.tracfoundation.org/t64tech.htm)
and
[TRAC](https://en.wikipedia.org/wiki/TRAC%5F%28programming_language%29) on Wikipedia.

## Installation

The source code is C99 POSIX.1-2008. The only requirements are `gcc`
and `make`. To build, run `make all`. Optimization flags can be set
using the make variable `OPTFLAGS`. To install, run `make
install`. Installation flags are, as usual, `BINDIR`, `MANDIR` and
`DESTDIR`.

## Usage

TRAC is started using the program `trac64`. To exit, press `CTRL-C` or
issue the primitive `#(HL)`.  A single filename on the command line is
loaded as a script that is executed immediately.  After the script is
finished, the idling script is executed as usual.

- `-e` : Extensions are enabled. Currently, the only extension is the
primitive `??`, which displays a help text with the list of
primitives.
- `-c` : ANSI coloring is enabled.
- `-q` : The banner is not displayed.
- `-i` *FILE* : The specified file is used as the idling script.

## Implementation specifics

### Integers

Integers are represented by the C `long` type. The primitives `AD`,
`SU` and `DV` do not generate exceptions, the primitive `DV` generates
an exception only on division by zero.

### Booleans

By default, Booleans are in octal, as per the T-64
definition. However, as octal representation is mostly used on 18-bit
or 36-bit architectures (such as the PDP-10), binary and hexadecimal
representation can be used. To switch between bases, use the calls
`#(MO,BIN)`, `#(MO,HEX)` and `#(MO,OCT)`.

### Blocks

Block addresses are platform specific paths. If a non-existing form
name is used, the path is a filename generated from the form name:
0-9, a-z and A-Z are are taken literally, other ASCII characters are
translated to URL encoding. Additionally an extension `.blk` is
added. Thus, the block name `/b?.` is encoded as the filename
`%2Fb%3F%2E.blk`.

### Colors

If the command line option `-c` is used, output is colorized in
several ways. There are different colors for: output (`PS` primitive),
banner, tracing (`TN` primitive) and diagnostic messages, (`SB`, `FB`,
`EB` etc.).

### Modes

As required, `#(MO)` switches to standard T-64: it displays `<T64>`,
disables extensions and sets the Boolean base to octal. `#(MO,E)`
enables extensions. The other mode switches are:

- `#(MO,COLOR)` : Enables ANSI coloring.
- `#(MO,NOCOLOR)` : Disables ANSI coloring.
- `#(MO,HEX)` : Switches to Boolean hexdecimal base.
- `#(MO,OCT)` : Switches to Boolean octal base.
- `#(MO,BIN)` : Switches to Boolean binary base.

