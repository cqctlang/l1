/* this needs to be revised:
 * - can logceil be done without iteration?
 * - separate BP into address in (bytes) and offset in (bits)
 *   to avoid overflow
 * - make documentation intelligible
*/

#include "sys.h"
#include "util.h"
#include "syscqct.h"

static int
logceil(int n)
{
	int i, j, logfloor = 0;
	int bitcount = 0;
	for(i = 0; i < 8*sizeof(int); i++){
		j = n>>i;
		if(j&1){
			bitcount++;
			logfloor = i;
		}
		if(j == 0)
			break;
	}
	if(bitcount > 1)
		return logfloor+1;
	else
		return logfloor;
}

enum {
	MAX_ADDRESSABLE_UNIT=8,
};

/*
 * bfg->bp = bit position in structure
 * bfg->bs = bit size of field
 * bo = bit offset within byte
 * lob = within containing structure, low-order byte of bit field
 * hob =  within containing structure, high-order byte of bit field
 * bc = number of bytes that contain any part of bit field
 * mls = minimum load size (in bytes) for a particular byte count
 * als = mls * (1 + ( (lob % mls) + bc > mls ? 1: 0))
 */
int
bitfieldgeom(BFgeom *bfg)
{
	Imm lob, hob;
	int bo, bc, sb, mls, als, bes, les;

	bo = bfg->bp%8;
	lob = bfg->bp/8;
	hob = (bfg->bp+bfg->bs-1)/8;
	bc = hob-lob+1;

	mls = 1<<logceil(bc);
	als = mls;

	if(als > MAX_ADDRESSABLE_UNIT)
		return -1;

	while(als < MAX_ADDRESSABLE_UNIT){
		if((lob%als)+bc > als)
			als *= 2;
		else
			break;
	}

	if(lob/als != hob/als)
		return -1;

	sb = lob-(lob%als);
	les = bo+8*(lob-sb);
	bes = 8*als-(bo+bfg->bs+8*(lob-sb));

	bfg->addr = sb;
	bfg->cnt = als;
	bfg->les = les;
	bfg->bes = bes;

//	xprintf("geom: addr=%lld cnt=%lld les=%d bp=%lld bs=%lld lob=%d\n",
//		bfg->addr, bfg->cnt, bfg->les, bfg->bp, bfg->bs, lob);

	return 0;
}

/* S points to bytes, as specified by BFG, containing bitfield.
   Return the value of the bitfield. */
Imm
bitfieldget(char *s, BFgeom *bfg)
{
	Imm v, x;
	unsigned i;
	unsigned char *p;

	p = (unsigned char*)s;
	v = 0;
	if(bfg->isbe){
		for(i = 0; i < bfg->cnt; i++){
			v <<= 8;
			v |= p[i];
		}
		v >>= bfg->bes;
	}else{
		x = 1;
		for(i = 0; i < bfg->cnt; i++){
			v |= p[i]*x;
			x *= 256;
		}
		v >>= bfg->les;
	}
	v &= (1ULL<<bfg->bs)-1;
	return v;
}

Imm
bitfieldput(char *s, BFgeom *bfg, Imm val)
{
	Imm w, m, x;
	int i;
	unsigned char *p;

	p = (unsigned char*)s;
	m = (1ULL<<bfg->bs)-1;
	val &= m;
	w = 0;
	if(bfg->isbe){
		for(i = 0; i < bfg->cnt; i++){
			w <<= 8;
			w |= p[i];
		}
		w &= ~(m<<bfg->bes);
		w |= val<<bfg->bes;
		for(i = bfg->cnt-1; i >= 0; i--){
			p[i] = w&0xff;
			w >>= 8;
		}
	}else{
		x = 1;
		for(i = 0; i < bfg->cnt; i++){
			w |= p[i]*x;
			x *= 256;
		}
		w &= ~(m<<bfg->les);
		w |= val<<bfg->les;
		for(i = 0; i < bfg->cnt; i++){
			p[i] = w&0xff;
			w >>= 8;
		}
	}
	return val;
}
