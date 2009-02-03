# put defaults here
CC        = gcc
LD        = ld
CFLAGS    = -Wall -g
CONF     ?= unix
L1LDFLAGS = -lpthread -lz -lcrypto
TARG      = l1

# put config-specific stuff in mk.$(CONF)
-include mk.$(CONF)

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
	$(L1_EXTRAS)

FNS_DECLS = $(foreach fn, $(L1_FUNS), "void $(fn)(Env *);")
FNS_CALLS = $(foreach fn, $(L1_FUNS), "	$(fn)(env);")

fns.$(CONF).c: $(L1_EXTRAS)
	@echo '#include "sys.h"' > $@
	@echo '#include "util.h"' >> $@
	@echo '' >> $@
	@echo '#include "syscqct.h"' >> $@
	@echo '' >> $@
	@echo $(FNS_DECLS) >> $@
	@echo '' >> $@
	@echo 'void' >> $@
	@echo 'fns(Env *env)' >> $@
	@echo '{' >> $@
	@echo $(FNS_CALLS) >> $@
	@echo '}' >> $@


L1O = $(L1C:.c=.o)

c.tab.c: c.y $(HDR)
	bison -d c.y

c.tab.h: c.tab.c

# -s drops fwrite dependency
lex.yy.c: c.l $(HDR)
	flex -f -s c.l

l1: l1.o main.o
	$(CC) -o $@ $^ $(L1LDFLAGS)
#	dwarf2cqct < l1 > l1.names

l1.o: c.tab.o lex.yy.o $(L1O)
	$(LD) -d -r -o $@ $^

lo: lo.o l1.o
	$(CC) -o $@ $^ $(L1LDFLAGS)

-include depend
depend: $(L1C)
	gcc $(INC) -MM $(L1C) > depend

archive:
	git archive --format=tar --prefix=l1/ HEAD > ../l1.tar

git.tar:
	tar -C .. -cz l1 > ../l1.git.tar.gz

clean:
	$(RM) *~ .gdbhistory core core.* callgrind.out.* vgcore.* test/core test/core.* test/callgrind.out.* test/vgcore.* test/*.failed test/*.vgfailed test/aqsort lex.yy.* *.tab.* c.output main.o lo lo.o l1.o fns.$(CONF).c $(L1O) $(TARG)  depend 
