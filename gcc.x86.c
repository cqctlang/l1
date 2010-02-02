#include "sys.h"

u64
rdtsc(void)
{
	u32 hi, lo;
	asm("rdtsc" : "=a"(lo), "=d"(hi));
	return (u64)lo|((u64)hi<<32);
}

void*
read_and_clear(void *pp)
{
	void **p = (void**)pp;
	void *q;
	q = 0;
	asm("xchg %0,%1" : "=r"(q), "=m"(*p) : "0"(q), "m"(*p) : "memory");
	return q;
}

void
atomic_inc(int *p)
{
	asm("lock incl %0" : "+m" (*p));
}

void
atomic_dec(int *p)
{
	asm("lock decl %0" : "+m" (*p));
}
