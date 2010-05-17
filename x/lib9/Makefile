CC        ?= gcc
LD        ?= ld
INC	  = -I../.. -I../include
CFLAGS   += $(INC)
TARG      = lib9.a

all: $(TARG)

C=\
	u16.c\
	u32.c\
	u64.c\
	lrand.c\
	nrand.c\

O = $(C:.c=.o)

FMTC=\
	fmt/dofmt.c\
	fmt/errfmt.c\
	fmt/fltfmt.c\
	fmt/fmt.c\
	fmt/fmtfd.c\
	fmt/fmtfdflush.c\
	fmt/fmtlocale.c\
	fmt/fmtlock.c\
	fmt/fmtnull.c\
	fmt/fmtprint.c\
	fmt/fmtquote.c\
	fmt/fmtrune.c\
	fmt/fmtstr.c\
	fmt/fmtvprint.c\
	fmt/fprint.c\
	fmt/nan64.c\
	fmt/print.c\
	fmt/runefmtstr.c\
	fmt/runeseprint.c\
	fmt/runesmprint.c\
	fmt/runesnprint.c\
	fmt/runesprint.c\
	fmt/runevseprint.c\
	fmt/runevsmprint.c\
	fmt/runevsnprint.c\
	fmt/seprint.c\
	fmt/smprint.c\
	fmt/snprint.c\
	fmt/sprint.c\
	fmt/strtod.c\
	fmt/vfprint.c\
	fmt/vseprint.c\
	fmt/vsmprint.c\
	fmt/vsnprint.c\
	fmt/charstod.c\
	fmt/pow10.c\

FMTO = $(FMTC:.c=.o)

UTFC=\
	utf/rune.c\
	utf/runestrcat.c\
	utf/runestrchr.c\
	utf/runestrcmp.c\
	utf/runestrcpy.c\
	utf/runestrdup.c\
	utf/runestrlen.c\
	utf/runestrecpy.c\
	utf/runestrncat.c\
	utf/runestrncmp.c\
	utf/runestrncpy.c\
	utf/runestrrchr.c\
	utf/runestrstr.c\
	utf/runetype.c\
	utf/utfecpy.c\
	utf/utflen.c\
	utf/utfnlen.c\
	utf/utfrrune.c\
	utf/utfrune.c\
	utf/utfutf.c\

UTFO = $(UTFC:.c=.o)

-include depend
depend: $(C) Makefile
	$(V)gcc $(INC) -MM $(C) > depend

$(TARG): $(O) $(UTFO) $(FMTO)
	$(AR) rsc $@ $^

clean:
	$(V)$(RM) depend $(TARG) *~ $(O) $(UTFO) $(FMTO) core core.*
