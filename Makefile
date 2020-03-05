NAME=trac64-1.0
BINDIR=/usr/bin
MANDIR=/usr/share/man

all: trac64

trac64:
	make -C src trac64

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	install -m0755 src/trac64 $(DESTDIR)$(BINDIR)/trac64
	mkdir -p $(DESTDIR)$(MANDIR)/man1
	install -m0644 man/trac64.1 $(DESTDIR)$(MANDIR)/man1/trac64.1

tar: clean
	mkdir -p ${NAME}
	cp -a AUTHORS LICENSE Makefile README.md src man ${NAME}
	tar zcf ${NAME}.tar.gz --owner=0 --group=0 ${NAME}
	rm -rf ${NAME}

clean:
	make -C src clean
	rm -fr ${NAME}
