CC        = gcc
LD        = ld
CFLAGS    = -Wall -g

TARG = clones forks list killself killself2

all: $(TARG)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clones: clones.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread
	dwarf2cqct < $@ > $@.names

forks: forks.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

list: list.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

killself: killself.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

killself2: killself2.o
	$(CC) $(CFLAGS) -o $@ $^
	dwarf2cqct < $@ > $@.names

clean:
	$(V)rm -rf *~ .gdbhistory core core.* *.o *.names $(TARG) *.so *.dylib depend
