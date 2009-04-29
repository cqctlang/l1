CC        ?= gcc
LD        ?= ld
INC	  = -I../.. -I../include
CFLAGS   += $(INC)
TARG      = libmp.a

all: $(TARG)

# removed: 	mpfmt.c

C=\
	mpaux.c\
	strtomp.c\
	mptobe.c\
	mptole.c\
	betomp.c\
	letomp.c\
	mpadd.c\
	mpsub.c\
	mpcmp.c\
	mpfactorial.c\
	mpmul.c\
	mpleft.c\
	mpright.c\
	mpvecadd.c\
	mpvecsub.c\
	mpvecdigmuladd.c\
	mpveccmp.c\
	mpdigdiv.c\
	mpdiv.c\
	mpexp.c\
	mpmod.c\
	mpextendedgcd.c\
	mpinvert.c\
	mprand.c\
	crt.c\
	mptoi.c\
	mptoui.c\
	mptov.c\
	mptouv.c\

O = $(C:.c=.o)

-include depend
depend: $(C) Makefile
	@echo depend libmp
	$(V)gcc $(INC) -MM $(C) > depend

libmp.a: $(O)
	$(AR) rsc $@ $^

clean:
	$(V)$(RM) depend libmp.a *~ $(O) core core.*
