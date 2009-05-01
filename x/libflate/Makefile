CC        ?= gcc
LD        ?= ld
INC	  = -I../.. -I../include
CFLAGS   += $(INC)
TARG      = libflate.a

all: $(TARG)

C=\
	deflate.c\
	deflatezlib.c\
	deflateblock.c\
	deflatezlibblock.c\
	inflate.c\
	inflatezlib.c\
	inflateblock.c\
	inflatezlibblock.c\
	flateerr.c\
	crc.c\
	adler.c\

O = $(C:.c=.o)

-include depend
depend: $(C) Makefile
	$(V)gcc $(INC) -MM $(C) > depend

libflate.a: $(O)
	$(AR) rsc $@ $^

clean:
	$(V)$(RM) depend libflate.a *~ $(O) core core.*
