srcdir = .

CC=clang
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

CFLAGS += -O2 -flto -std=c99 -Wall -Wextra -Weverything
CFLAGS += -Wno-unsafe-buffer-usage -Wno-format-nonliteral

ldlibs = $(LDLIBS)

objs = main.o

all: urxvt_alpha

.PHONY: all clean install uninstall
.SUFFIXES:
.SUFFIXES: .c .o

urxvt_alpha: $(objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(objs) $(ldlibs)

$(objs): Makefile

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o urxvt_alpha

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp urxvt_alpha $(DESTDIR)$(PREFIX)/bin/
	chmod 755 $(DESTDIR)$(PREFIX)/bin/urxvt_alpha
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp urxvt_alpha.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/urxvt_alpha.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/urxvt_alpha
	rm -f $(DESTDIR)$(MANPREFIX)/man1/urxvt_alpha.1
