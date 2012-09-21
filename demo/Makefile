CC        = gcc
LD        = ld
CFLAGS    = -Wall -g

TARG =\
	forkexec\
	list\
	listbug\

all: $(TARG)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

forkexec: forkexec.o
	$(CC) $(CFLAGS) -o $@ $^

list: list.o
	$(CC) $(CFLAGS) -o $@ $^

listbug: listbug.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(V)rm -rf *~ .gdbhistory core core.* *.o *.names $(TARG) *.so *.dylib depend
