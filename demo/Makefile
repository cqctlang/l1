CC        = gcc
LD        = ld
CFLAGS    = -Wall -g

TARG = clones forks list

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

clean:
	rm -rf *~ .gdbhistory core core.* *.o *.names $(TARG) *.so *.dylib depend
