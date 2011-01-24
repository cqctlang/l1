#include "sys.h"
#include "util.h"
#include "syscqct.h"

Pair*
mkpair(Val a, Val d)
{
	Pair *p;
	p = (Pair*)malq(Qpair);
	// setc*r would invoke write barrier
	p->car = a;
	p->cdr = d;
	return p;
}
