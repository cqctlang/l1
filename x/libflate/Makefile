CC        = gcc
LD        = ld
INC	  = -I../..
CFLAGS    = -Wall -g $(INC)
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
depend: $(C)
	gcc $(INC) -MM $(C) > depend

libflate.a: $(O)
	ar -r $@ $^

clean:
	$(RM) *~ $(O) core core.*

