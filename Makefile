CC        = gcc
LD        = ld
CFLAGS    = -Wall -g
CONF     ?= unix
L1LIBS	  = -lpthread
L1DEPS    =
TARG      = l1

.DEFAULT_GOAL := all

-include mk.$(CONF)

all: $(TARG)

HDR = sys.h util.h cqct.h syscqct.h 
L1C =\
	hash.c\
	util.c\
	printexpr.c\
	bitfield.c\
	parse.c\
	xenv.c\
	cutil.c\
	compilens.c\
	compile0.c\
	compile1.c\
	compile2.c\
	type.c\
	cg.c\
	vm.c\
	cqct.c\
	fns.$(CONF).c\
	$(L1EXTRAS)

L1O = $(L1C:.c=.o)

FNSDECLS = $(foreach fn, $(L1FUNS), "void $(fn)(Env *);")
FNSCALLS = $(foreach fn, $(L1FUNS), "	$(fn)(env);")

fns.$(CONF).c: $(L1EXTRAS)
	@echo '#include "sys.h"' > $@
	@echo '#include "util.h"' >> $@
	@echo '' >> $@
	@echo '#include "syscqct.h"' >> $@
	@echo '' >> $@
	@echo $(FNSDECLS) >> $@
	@echo '' >> $@
	@echo 'void' >> $@
	@echo 'fns(Env *env)' >> $@
	@echo '{' >> $@
	@echo $(FNSCALLS) >> $@
	@echo '}' >> $@

c.tab.c: c.y $(HDR)
	bison -d c.y

c.tab.h: c.tab.c

# -s drops fwrite dependency
lex.yy.c: c.l $(HDR)
	flex -f -s c.l

l1: l1.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(L1LIBS)
#	dwarf2cqct < l1 > l1.names

l1.o: c.tab.o lex.yy.o $(L1O) $(L1DEPS)
	$(LD) -d -r -o $@ $^ $(L1DEPS)

libl1.so: CFLAGS += -fPIC -nostdlib

libl1.so: l1.o
	$(CC) -shared -Xlinker -Bsymbolic -o $@ $^

-include depend
depend: $(L1C) Makefile
	gcc $(INC) -MM $(L1C) > depend

archive:
	git archive --format=tar --prefix=l1/ HEAD > ../l1.tar

git.tar:
	tar -C .. -cz l1 > ../l1.git.tar.gz

clean:
	$(MAKE) -C x/lib9 clean
	$(MAKE) -C x/libflate clean
	$(MAKE) -C x/libsec clean
	$(RM) *~ .gdbhistory core core.* callgrind.out.* vgcore.* test/core test/core.* test/callgrind.out.* test/vgcore.* test/*.failed test/*.vgfailed test/aqsort lex.yy.* *.tab.* c.output l1.names main.o l1.o fns.*.c *.o $(TARG) *.so depend
