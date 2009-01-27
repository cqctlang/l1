CC = gcc
LD = ld
CFLAGS = -Wall -g
CONF = unix

libl1.so: CFLAGS += -fPIC -nostdlib

TARG = l1
all: $(TARG)

HDR = sys.h util.h cqct.h syscqct.h 
L1C =\
	hash.c\
	util.c\
	printexpr.c\
	bitfield.c\
	parse.c\
	compilec.c\
	compile0.c\
	cutil.c\
	compile.c\
	vm.c\
	fns.$(CONF).c\

L1C += $(shell awk -v 'CONF='$(CONF) -f ./mkconf < conf.$(CONF))

L1O = $(L1C:.c=.o)

c.tab.c: c.y $(HDR)
	bison -d c.y

c.tab.h: c.tab.c

# -s drops fwrite dependency
lex.yy.c: c.l $(HDR)
	flex -f -s c.l

l1: l1.o main.o
	$(CC) -o $@ $^ -lpthread -lz -lssl
#	dwarf2cqct < l1 > l1.names

l1.o: c.tab.o lex.yy.o $(L1O)
	$(LD) -d -r -o $@ $^

libl1.so: c.tab.o lex.yy.o $(L1O)
	$(CC) -nostdlib -shared -Xlinker -Bsymbolic -o $@ $^

lo.o: lo.c
	$(CC) -g -Wall -fPIC -c $^

lo: lo.o libl1.so
	$(CC) -g -o lo lo.o -L. -ll1

-include depend
depend: Makefile
	gcc $(INC) -MM $(L1C) > depend

archive:
	git archive --format=tar --prefix=l1/ HEAD > ../l1.tar

git.tar:
	tar -C .. -cz l1 > ../l1.git.tar.gz

clean:
	rm -f *~ .gdbhistory core core.* callgrind.out.* vgcore.* test/core test/core.* test/callgrind.out.* test/vgcore.* test/*.failed test/*.vgfailed test/aqsort lex.yy.* *.tab.* c.output main.o lo lo.o l1.o fns.$(CONF).c $(L1O) $(TARG) libl1.so depend 
