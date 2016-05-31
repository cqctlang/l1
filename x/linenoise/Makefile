CC        ?= gcc
LD        ?= ld
CFLAGS += $(INC)
TARG = liblinenoise.a

all: $(TARG)

C=\
  linenoise.c

O = $(C:.c=.o)

-include depend
depend: $(C) Makefile
	$(V)gcc -MM $(C) > depend

$(TARG): $(O)
	$(AR) rsc $@ $^

clean:
	$(V)$(RM) depend $(TARG) $(O)

