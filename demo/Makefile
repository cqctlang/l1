CC        = gcc
LD        = ld
CFLAGS    = -Wall -g

TARG = clones\
	forks\
	list\
	sig\
	attach\
	par1\
	exit\
	locals1 locals1.32\
	wind1 wind1.32\
	wind2 wind2.32\


all: $(TARG)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

mclones.o: mclones.c
	$(CC) -m32 $(CFLAGS) -c -o $@ $<

mclones32: mclones.o
	$(CC) -m32 $(CFLAGS) -o $@ $^ -lpthread
	dwarf2cqct < $@ > $@.names

clones: clones.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread
	dwarf2cqct < $@ > $@.names

forks: forks.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

list: list.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

sig: sig.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

attach: attach.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

par1: par1.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

locals1: locals1.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

locals1.32: locals1.c
	cc -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

wind1: wind1.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

wind1.32: wind1.c
	cc -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

wind2: wind2.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

wind2.32: wind2.c
	cc -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

exit: exit.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

list64: list.c
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

list32: list.c
	cc -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

list32stat: list.c
	$(CC) -static -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

clean:
	$(V)rm -rf *~ .gdbhistory core core.* *.o *.names $(TARG) *.so *.dylib depend
