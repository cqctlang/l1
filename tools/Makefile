CC        = gcc
LD        = ld
CFLAGS    = -Wall -g

TARG =\
	mkunix\
	slayer\

all: $(TARG)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(V)rm -rf *~ .gdbhistory core core.* *.o *.names $(TARG) *.so *.dylib depend
