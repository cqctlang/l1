CC = gcc
CFLAGS = -Wall -g

TARG = l1
all: $(TARG)

HDR = sys.h util.h l1.h code.h
C = l1.c compile0.c compile.c vm.c hash.c util.c main.c printexpr.c
O = $(C:.c=.o)

c.tab.c: c.y $(HDR)
	bison -d c.y

c.tab.h: c.tab.c

lex.yy.c: c.l $(HDR)
	flex c.l

l1: c.tab.o lex.yy.o $(O)
	$(CC) -o $@ $^  -lfl -lpthread

-include depend
depend: Makefile
	gcc $(INC) -MM $(C) > depend

clean:
	rm -f *~ .gdbhistory core core.* callgrind.out.* vgcore.* test/core test/core.* test/callgrind.out.* test/vgcore.* test/*.failed test/*.vgfailed lex.yy.* *.tab.* $(O) $(TARG) depend 
