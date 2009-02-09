CC        = gcc
LD        = ld
INC	  = -I../.. -I../include
CFLAGS   += $(INC)
TARG      = lib9.a

all: $(TARG)

C=\
	u16.c\
	u32.c\
	u64.c\

O = $(C:.c=.o)

-include depend
depend: $(C) Makefile
	gcc $(INC) -MM $(C) > depend

$(TARG): $(O)
	ar rsc $@ $^

clean:
	$(RM) depend $(TARG) *~ $(O) core core.*
