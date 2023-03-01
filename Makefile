.PHONY: all test install clean

prefix = /usr/local
exec_prefix = $(prefix)
libdir = $(exec_prefix)/lib
includedir = $(prefix)/include

CC = cc
INSTALL = install

ALL = libchtrie.a chtrie.o

all: $(ALL)

chtrie.o: chtrie.c chtrie.h
	$(CC) $(CFLAGS) -c -o $@ $<

libchtrie.a: chtrie.o
	ar -cr $@ $^

run.tmp: chtrie.o chtrie.h test.c
	$(CC) $(CFLAGS) -o $@ chtrie.o test.c

test: run.tmp
	./runtest

install: all
	$(INSTALL) -d $(includedir) $(libdir)
	$(INSTALL) chtrie.h $(includedir)/
	$(INSTALL) libchtrie.a $(libdir)/

clean:
	rm -rf $(ALL) *.tmp
