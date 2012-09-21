CC        = gcc
LD        = ld
CFLAGS    += -Wall -g -O3
CPPFLAGS  +=
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
export CPPFLAGS
export V

all: $(TARG)

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
	qc.c\
	cpe.c\
	cpq.c\
	cpx.c\
	cpn.c\
	cpm.c\
	cpw.c\
	cpa.c\
	cpc.c\
	cp0.c\
	cpg.c\
	cpk.c\
	cpl.c\
	cpi.c\
	cpr.c\
	cp1.c\
	cpv.c\
	cpb.c\
	cg.c\
	ch.c\
	cid.c\
	ctype.c\
	cval.c\
	list.c\
	pair.c\
	rec.c\
	str.c\
	stx.c\
	sym.c\
	tab.c\
	vec.c\
	mem.c\
	vm.c\
	cqct.c\
	xfd.c\
	$(MACH)\
	fns.$(CONF).c\
	$(L1EXTRAS)

L1O = $(L1C:.c=.o)

L1DEPS += x/lib9/lib9.a
L1FUNS += fnfmt fnjson
L1EXTRAS += fnfmt.c fnjson.c

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

FORCE:
x/lib9/lib9.a: FORCE
	$(V)$(MAKE) -s -C x/lib9

parser:
	@echo + flex $<
	$(V)flex -f -s c.l # -s drops fwrite dependency
	@echo + bison $<
	$(V)bison -d c.y

%.o:%.c
	@echo + cc $<
	$(V)$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

l1: l1.o main.o
	@echo + ld $@
	$(V)$(CC) $(CFLAGS) -o $@ $^ $(L1LIBS)

l1s: l1.o main.o
	@echo + ld $@
	$(V)$(CC) -static $(CFLAGS) -o $@ $^ $(L1LIBS)

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
	git archive --format=tar --prefix=l1/ HEAD | gzip -c > ../l1.tar.gz

git.tar:
	tar -C .. -cz l1 > ../l1.git.tar.gz

# it is irritating that we reproduce demo dependencies here.
DEMO=\
	demo/forkexec.c\
	demo/list.c\
	demo/listbug.c

doc: doc/debug.html

doc/debug.html: doc/debug.src.html $(DEMO)
	@$(MAKE) -C demo
	@$(MAKE) -C doc

clean:
	@$(MAKE) -s -C x/lib9 clean
	@$(MAKE) -s -C x/libflate clean
	@$(MAKE) -s -C x/libsec clean
	@$(MAKE) -s -C demo clean
	@$(RM) *~ .gdbhistory core core.* callgrind.out.* vgcore.* test/core test/core.* test/callgrind.out.* test/vgcore.* test/*.failed test/*.vgfailed c.output l1.names main.o l1.o fns.*.c *.o $(TARG) *.so *.dylib depend
