CC = gcc
CFLAGS = -Wall -g 

TARG = l1
all: $(TARG)

HDR = sys.h util.h cqct.h syscqct.h 
C =\
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
	main.c
O = $(C:.c=.o)

c.tab.c: c.y $(HDR)
	bison -v -d c.y

c.tab.h: c.tab.c

lex.yy.c: c.l $(HDR)
	flex c.l

l1: c.tab.o lex.yy.o $(O)
	$(CC) -o $@ $^ -lfl -lpthread

-include depend
depend: Makefile
	gcc $(INC) -MM $(C) > depend

clean:
	rm -f *~ .gdbhistory core core.* callgrind.out.* vgcore.* test/core test/core.* test/callgrind.out.* test/vgcore.* test/*.failed test/*.vgfailed lex.yy.* *.tab.* c.output $(O) $(TARG) depend 
