CC        ?= gcc
LD        ?= ld
CFLAGS    ?= -Wall -g
LDFLAGS   ?= -r
LIBS	  ?=
TARG      = udis.o
V	  ?= @

all: $(TARG)

C=\
	decode.c\
	input.c\
	itab.c\
	syn-att.c\
	syn-intel.c\
	syn.c\
	udis86.c

O=$(C:.c=.o)

%.o:%.c
#	@echo + cc $<
	$(V)$(CC) $(CFLAGS) -c -o $@ $<

udis.o: $(O)
#	@echo + ld $@
	$(V)$(LD) $(LDFLAGS) -o $@ $^

-include depend
depend: $(C) Makefile
	$(V)gcc $(INC) -MM $(C) > depend

clean:
	$(V)rm -f *~ .gdbhistory core core.* *.o depend
