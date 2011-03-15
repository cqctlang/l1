CC        = gcc
LD        = ld
CFLAGS    += -Wall -g
LDFLAGS   = -r
CONF     ?= unix
L1LIBS	  =
L1DEPS    =
TARG      = l1
V	  = @

.DEFAULT_GOAL := all

-include mk.$(CONF)

export CC
export CFLAGS
export V

all: $(TARG)

# phony target to ensure libraries get remade.
FORCE: 

MACH=
HDR = sys.h util.h cqct.h syscqct.h
L1C =\
	lex.yy.c\
	c.tab.c\
	hash.c\
	util.c\
	printexpr.c\
	bitfield.c\
	parse.c\
	xenv.c\
	cutil.c\
	compileq.c\
	compilens.c\
	compilea.c\
	compile0.c\
	compileg.c\
	compilel.c\
	compilei.c\
	compile1.c\
	compile2.c\
	compilev.c\
	compileb.c\
	compileu.c\
	compilex.c\
	compilec.c\
	compiles.c\
	type.c\
	cg.c\
	ch.c\
	list.c\
	pair.c\
	rec.c\
	tab.c\
	vec.c\
	type.c\
	mem.c\
	vm.c\
	cqct.c\
	xfd.c\
	$(MACH)\
	fns.$(CONF).c\
	$(L1EXTRAS)

L1O = $(L1C:.c=.o)

L1DEPS += x/lib9/lib9.a
L1FUNS += fnfmt
L1EXTRAS += fnfmt.c

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

x/lib9/lib9.a: FORCE
	$(MAKE) -s -C x/lib9

parser:
	@echo + flex $<
	$(V)flex -f -s c.l # -s drops fwrite dependency
	@echo + bison $<
	$(V)bison -d c.y

%.o:%.c
	@echo + cc $<
	$(V)$(CC) $(CFLAGS) -c -o $@ $<

l1: l1.o main.o
	@echo + ld $@
	$(V)$(CC) $(CFLAGS) -o $@ $^ $(L1LIBS)

l1.o: $(L1O) $(L1DEPS)
	@echo + ld $@
	$(V)$(LD) $(LDFLAGS) -o $@ $^

libl1.so: CFLAGS += -fPIC -nostdlib

libl1.so: l1.o
	@echo + ld $@
	$(V)$(CC) -shared -Xlinker -Bsymbolic -o $@ $^

libl1.dylib: l1.o
	@echo + ld $@
	if [ ! -f libgcc_s.dylib ] ; then ln -s /usr/lib/libgcc_s.1.dylib libgcc_s.dylib ; fi
	libtool -dynamic -lc -L. -lgcc_s -o $@ $^

-include depend
depend: $(L1C) Makefile
	$(V)gcc $(INC) -MM $(L1C) > depend

archive:
	git archive --format=tar --prefix=l1/ HEAD > ../l1.tar

git.tar:
	tar -C .. -cz l1 > ../l1.git.tar.gz

clean:
	@$(MAKE) -C x/lib9 clean
	@$(MAKE) -C x/libflate clean
	@$(MAKE) -C x/libsec clean
	@$(MAKE) -C demo clean
	@$(RM) *~ .gdbhistory core core.* callgrind.out.* vgcore.* test/core test/core.* test/callgrind.out.* test/vgcore.* test/*.failed test/*.vgfailed c.output l1.names main.o l1.o fns.*.c *.o $(TARG) *.so *.dylib depend
