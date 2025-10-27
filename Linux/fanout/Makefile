# @(#)Makefile 1.2 12/13/99 lm@aix.bitmover.com

CFLAGS=-O

all: fanout

install:
	@make uninstall
	cp fanout /usr/bin
	chown root /usr/bin/fanout
	chmod 4755 /usr/bin/fanout

uninstall:
	/bin/rm -f /bin/fanout /sbin/fanout /usr/bin/fanout /usr/sbin/fanout

clobber clean:
	/bin/rm -f fanout a.out core
	bk clean
