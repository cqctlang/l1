CC        = gcc
LD        = ld
INC	  = -I../.. -I../include
CFLAGS   += $(INC)
TARG      = libsec.a

all: $(TARG)

# unported
# 	aes.c\
# 	blowfish.c\
# 	decodepem.c\
# 	des.c\
# 	des3CBC.c\
# 	des3ECB.c\
# 	desCBC.c\
# 	desECB.c\
# 	desmodes.c\
# 	dsaalloc.c\
# 	dsagen.c\
# 	dsaprimes.c\
# 	dsaprivtopub.c\
# 	dsasign.c\
# 	dsaverify.c\
# 	egalloc.c\
# 	egdecrypt.c\
# 	egencrypt.c\
# 	eggen.c\
# 	egprivtopub.c\
# 	egsign.c\
# 	egverify.c\
# 	fastrand.c\
# 	genprime.c\
# 	genrandom.c\
# 	gensafeprime.c\
# 	genstrongprime.c\
# 	hmac.c\
# 	md4.c\
# 	md5.c\
# 	md5block.c\
# 	md5pickle.c\
# 	nfastrand.c\
# 	prng.c\
# 	probably_prime.c\
# 	rc4.c\
# 	readcert.c\
# 	rsaalloc.c\
# 	rsadecrypt.c\
# 	rsaencrypt.c\
# 	rsafill.c\
# 	rsagen.c\
# 	rsaprivtopub.c\
#	sha1pickle.c\
# 	smallprimetest.c\
# 	thumb.c\
# 	tlshand.c\
# 	x509.c\

C=\
	sha1.c\
	sha1block.c\

O = $(C:.c=.o)

-include depend
depend: $(C) Makefile
	gcc $(INC) -MM $(C) > depend

$(TARG): $(O)
	ar rsc $@ $^

clean:
	$(RM) depend $(TARG) *~ *.o core core.*
