#include "sys.h"
#include "util.h"
#include "syscqct.h"

/*
  ops
  - alloc seg
  - clear seg
  - enum segs
  - map addr to seg
*/

typedef Seg Seg;

struct Seg
{
	void *addr, *scan;
	u32 len;
};

Seg*
mkseg(u32 len)
{
	Seg *s;
	if(len%4096)
		fatal("mkseg bug");
	s = emalloc(len);
	s->addr = emalloc(len);
	s->len = len;
	return s;
}
