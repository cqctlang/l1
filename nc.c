#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "amd64.h"

NC*	trampentry;

NC*
mknc()
{
	NC *nc;
	nc = emalloc(sizeof(NC));
	nc->max = 256;
	nc->p = nc->buf = emalloc(nc->max*sizeof(u8));
	return nc;
}

void
freenc(NC *nc)
{
	efree(nc);
}

void
ncclr(NC *nc)
{
	nc->p = nc->buf;
	nc->n = 0;
}

static void
nccap(NC *nc, u32 m)
{
	if(nc->max-nc->n >= m)
		return;
	nc->buf = erealloc(nc->buf, nc->max*sizeof(u8), 2*nc->max*sizeof(u8));
	nc->max *= 2;
	nc->p = nc->buf+nc->n;
	nccap(nc, m);
}

void
ncdis(NC *nc)
{
	disx86(nc->buf, nc->buf+nc->n, 0);
}

void
emitu8(NC *nc, u8 w)
{
	nccap(nc, sizeof(w));
	*(u8*)nc->p = w;
	nc->p += sizeof(w);
	nc->n += sizeof(w);
}

void
emitu16(NC *nc, u16 w)
{
	nccap(nc, sizeof(w));
	*(u16*)nc->p = w;
	nc->p += sizeof(w);
	nc->n += sizeof(w);
}

void
emitu32(NC *nc, u32 w)
{
	nccap(nc, sizeof(w));
	*(u32*)nc->p = w;
	nc->p += sizeof(w);
	nc->n += sizeof(w);
}

void
emitu64(NC *nc, u64 w)
{
	nccap(nc, sizeof(w));
	*(u64*)nc->p = w;
	nc->p += sizeof(w);
	nc->n += sizeof(w);
}

#if 0
static void
loadvm(NC *nc)
{
	/* load native context from VM (in RBX) */
	MOV(nc, RBP, indirect2(RBX, disp(0*8)));	/* fp */
	MOV(nc, R12, indirect2(RBX, disp(1*8)));	/* cl */
	MOV(nc, R13, indirect2(RBX, disp(2*8)));	/* vc */
	MOV(nc, R14, indirect2(RBX, disp(3*8)));	/* ac */
}
#endif

static void
savevm(NC *nc)
{
	/* save native context to VM (in RBX) */
	MOV(nc, indirect2(RBX, disp(0*8)), RBP);	/* fp */
	MOV(nc, indirect2(RBX, disp(1*8)), R12);	/* cl */
	MOV(nc, indirect2(RBX, disp(2*8)), R13);	/* vc */
	MOV(nc, indirect2(RBX, disp(3*8)), R14);	/* ac */
}

static NC*
mktrampentry()
{
	NC *nc;
	nc = mknc();

	savevm(nc);
	MOV(nc, RDI, RBX);	/* vm argument */
	XOR(nc, RAX, RAX);
	MOV(nc, RAX, imm(0));	/* abs addr ncallret */
	PUSH(nc, RAX);
	MOV(nc, RAX, imm(0));	/* abs addr ncall */
	JMP(nc, RAX);

	return nc;
}

void
initnc()
{
	trampentry = mktrampentry();
//	printf("trampentry:\n");
//	ncdis(trampentry);
}

void
fininc()
{
	freenc(trampentry);
}
