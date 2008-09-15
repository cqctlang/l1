CC = gcc
CFLAGS = -Wall -g

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
	noix.c\

#L1C +=\
#	fnfs.c\
#	fnio.c\
#	fnnet.c\
#	fnsys.c

L1O = $(L1C:.c=.o)

c.tab.c: c.y $(HDR)
	bison -d c.y

c.tab.h: c.tab.c

# -s drops fwrite dependency
lex.yy.c: c.l $(HDR)
	flex -f -s c.l

l1: c.tab.o lex.yy.o main.o $(L1O)
	$(CC) -o $@ $^ -lfl -lpthread

libl1.so: c.tab.o lex.yy.o $(L1O)
	$(CC) -nostdlib -shared -Xlinker -Bsymbolic -o $@ $^

-include depend
depend: Makefile
	gcc $(INC) -MM $(L1C) > depend

clean:
	rm -f *~ .gdbhistory core core.* callgrind.out.* vgcore.* test/core test/core.* test/callgrind.out.* test/vgcore.* test/*.failed test/*.vgfailed test/aqsort lex.yy.* *.tab.* c.output main.o $(L1O) $(TARG) libl1.so lo depend 
