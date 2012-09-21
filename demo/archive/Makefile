CC        = gcc
LD        = ld
CFLAGS    = -Wall -g

TARG = clones\
	forks\
	forkexec\
	list\
	listbug\
	sig\
	attach\
	par1\
	exit\
	locals1 locals1.32\
	wind1 wind1.32\
	wind2 wind2.32\
	float1 float1.32\
	libA.so\
	uselib\
	fault\
	list64\
	list64.static\
	list64.pie\
#	list64.pie.static\
#	types1 types1.32\

TARG32 = \
	list32\
	list32.pie\
	list32.static\
#	list32.pie.static\
	mclones32\

all: $(TARG) $(TARG32)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

mclones.o: mclones.c
	$(CC) -m32 $(CFLAGS) -c -o $@ $<

mclones32: mclones.o
	$(CC) -m32 $(CFLAGS) -o $@ $^ -lpthread

clones: clones.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

forks: forks.o
	$(CC) $(CFLAGS) -o $@ $^

forkexec: forkexec.o
	$(CC) $(CFLAGS) -o $@ $^

list: list.o
	$(CC) $(CFLAGS) -o $@ $^

listbug: listbug.o
	$(CC) $(CFLAGS) -o $@ $^

sig: sig.o
	$(CC) $(CFLAGS) -o $@ $^

fault: fault.o
	$(CC) $(CFLAGS) -o $@ $^

attach: attach.o
	$(CC) $(CFLAGS) -o $@ $^

par1: par1.o
	$(CC) $(CFLAGS) -o $@ $^

locals1.o: locals1.c locals1.h

locals1: locals1.o  locals1.xtra.o
	$(CC) $(CFLAGS) -o $@ $^

locals1.32: locals1.c locals1.xtra.c locals1.h
	cc -m32 -B/usr/lib32 $(CFLAGS) -o $@ locals1.c locals1.xtra.c

wind1: wind1.o
	$(CC) $(CFLAGS) -o $@ $^

wind1.32: wind1.c
	cc -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^

wind2: wind2.o
	$(CC) $(CFLAGS) -o $@ $^

wind2.32: wind2.c
	cc -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^

libA.so: libA.o
	$(CC) -shared -o $@ $^

uselib: uselib.o libA.so
	$(CC) -o $@ uselib.o -L. -lA

types1: types1.o
	$(CC) $(CFLAGS) -o $@ $^

types1.32: types1.c
	cc -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^

float1: float1.o
	$(CC) $(CFLAGS) -o $@ $^

float1.32: float1.c
	cc -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^

exit: exit.o
	$(CC) $(CFLAGS) -o $@ $^

list64: list.c
	$(CC) $(CFLAGS) -o $@ $^

list32: list.c
	$(CC) -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^

list64.pie: list.c
	$(CC) -fPIE -pie $(CFLAGS) -o $@ $^

list32.pie: list.c
	$(CC) -fPIE -pie -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^

list64.static: list.c
	$(CC) -static $(CFLAGS) -o $@ $^

list32.static: list.c
	$(CC) -static -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^

# we don't know how to build these

#list64.pie.static: list.c
#	$(CC) -static -fPIE -pie $(CFLAGS) -o $@ $^

#list32.pie.static: list.c
#	$(CC) -static -fPIE -pie -m32 -B/usr/lib32 $(CFLAGS) -o $@ $^

clean:
	$(V)rm -rf *~ .gdbhistory core core.* *.o *.names $(TARG) $(TARG32) *.so *.dylib depend
