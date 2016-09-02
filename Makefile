CC         = gcc
LD         = ld
CFLAGS    += -Wall -g -O3
CPPFLAGS  +=
LDFLAGS    = -r
ARCH	  := $(shell uname -m | sed 's/x86_64/amd64/')
OS	  := $(shell uname -s | tr '[:upper:]' '[:lower:]')
CONF      ?= $(OS)
L1LIBPATH  =
L1LIBS	   =
L1DEPS     =
L1FUNS     =
L1X        =
TARG       = l1
V	   = @
PREFIX	:= /usr/public
IDIR	   =

# relink options
RLTARG	   =
RLX	   =
HEAPIFY_HEAP =
HEAPIFY_ARGS =
HEAPIFY_CWD =$(shell pwd)
MAKE_CWD = $(shell pwd)

.DEFAULT_GOAL := all

all: $(TARG)

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
	cps.c\
	cpb.c\
	cpv.c\
	code.c\
	nc.c\
	cg.c\
	ch.c\
	cid.c\
	ctype.c\
	cval.c\
	list.c\
	pair.c\
	str.c\
	stx.c\
	sym.c\
	tab.c\
	vec.c\
	mem.c\
	boot.c\
	vm.c\
	cqct.c\
	xfd.c\
	fns.$(CONF).c\
	amd64.c

L1DEPS += x/lib9/lib9.a udis/udis.o x/linenoise/liblinenoise.a
L1FUNS += fnfmt fnjson fndis fnrepl
L1C += fnfmt.c fnjson.c fndis.c fnrepl.c

include conf/mk.$(CONF)

export CC
export LD
export CFLAGS
export LDFLAGS
export CPPFLAGS
export V

FNSDECLS = $(foreach fn, $(L1FUNS), "void $(fn)(Env);")
FNSCALLS = $(foreach fn, $(L1FUNS), "	$(fn)(env);")

fns.$(CONF).c:
	@echo '#include "sys.h"' > $@
	@echo '#include "util.h"' >> $@
	@echo '' >> $@
	@echo '#include "syscqct.h"' >> $@
	@echo '' >> $@
	@echo $(FNSDECLS) >> $@
	@echo '' >> $@
	@echo 'void' >> $@
	@echo 'fns(Env env)' >> $@
	@echo '{' >> $@
	@echo $(FNSCALLS) >> $@
	@echo '}' >> $@

FORCE:
x/lib9/lib9.a: FORCE
	$(V)$(MAKE) -s -C x/lib9

x/linenoise/liblinenoise.a:
	$(V)$(MAKE) -s -C x/linenoise

udis/udis.o:
	@echo + mk udis
	$(V)$(MAKE) -s -C udis

c.tab.c: c.y
	@echo + bison $<
	$(V)bison -d $<

lex.yy.c: c.l
	@echo + flex $<
	$(V)flex -f -s $< # -s drops fwrite dependency

%.o:%.c
	@echo + cc $<
	$(V)$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

l1.o: $(L1C:.c=.o) $(L1DEPS)
	@echo + ld $@
	$(V)$(LD) $(LDFLAGS) -o $@ $^

tl1: l1.o main.o
	@echo + ld $@
	$(V)$(CC) $(CFLAGS) -o $@ $^ $(L1LIBS)

tl1s: l1.o main.o
	@echo + ld $@
	$(V)$(CC) -static $(CFLAGS) -o $@ $^ $(L1LIBS)

libl1.a: l1.o main.o
	@echo + ld $@
	ar rcS libl1.a l1.o main.o $(RLX)

dummy:

heapify: dummy l1
	@echo + heapify $(HEAPIFY_HEAP)
	$(V)cd $(HEAPIFY_CWD) && $(RT)$(MAKE_CWD)/l1 -d $(HEAPIFY_HEAP) $(HEAPIFY_ARGS)

testasm: testasm.o amd64.o
	@echo + ld $@
	$(V)$(CC) $(CFLAGS) -o $@ $^

test: $(TARG) dummy
	cd test && ./test.py
	cd test && ./clitest

-include depend
depend: $(L1C) Makefile
	$(V)gcc $(INC) -MM $(L1C) > depend

archive:
	git archive --format=tar --prefix=l1/ HEAD | gzip -c > ../l1.tar.gz

git.tar:
	tar -C .. -cz l1 > ../l1.git.tar.gz

rpm:
	git archive --format=tar --prefix=l1/ HEAD | gzip -c > `rpmbuild -E '%{_topdir}' 2> /dev/null`/SOURCES/l1-3.11.tar.gz
	rpmbuild -ba l1.spec

# it is irritating that we reproduce demo dependencies here.
DEMO=\
	demo/forkexec.c\
	demo/list.c\
	demo/listbug.c

doc: doc/debug.html

doc/debug.html: doc/debug.src.html $(DEMO)
	@$(MAKE) -C demo
	@$(MAKE) -C doc

testclean:
	@$(RM) test/core test/core.* test/callgrind.out.* test/vgcore.* test/*.failed test/*.vgfailed

install: $(TARG)
	@mkdir -p $(PREFIX)/share/l1
	@cp $(TARG) $(PREFIX)/share/l1
	@cp -ap lib $(PREFIX)/share/l1
	@mkdir -p $(PREFIX)/bin
	@rm -f $(PREFIX)/bin/$(TARG)
	@ln -s ../share/l1/$(TARG) $(PREFIX)/bin/$(TARG)
# it breaks my heart to include this hack.
# this is absolute to accomodate some local automounter
# relative-link weirdness with per-arch bin directories
	if readlink -f $(PREFIX)/bin/$(TARG) > /dev/null ; then /bin/true ; else rm -f $(PREFIX)/bin/$(TARG) ; ln -s $(PREFIX)/share/l1/$(TARG) $(PREFIX)/bin/$(TARG) ; fi

clean: testclean
	@$(MAKE) -s -C x/lib9 clean
	@$(MAKE) -s -C x/libflate clean
	@$(MAKE) -s -C x/linenoise clean
	@$(MAKE) -s -C x/libsec clean
	@$(MAKE) -s -C udis clean
	@$(MAKE) -s -C demo clean
	@$(RM) *~ .gdbhistory core core.* callgrind.out.* vgcore.* c.output l1.names main.o l1.o fns.*.c *.o $(TARG) *.so *.dylib mkelf mks l1.heap heap.S depend tl1
