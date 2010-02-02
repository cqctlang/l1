CC        = gcc
LD        = ld
CFLAGS    = -Wall -g

TARG = mclones32 clones forks list sig

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

clean:
	$(V)rm -rf *~ .gdbhistory core core.* *.o *.names $(TARG) *.so *.dylib depend
