#include "sys.h"
#include "util.h"
#include "syscqct.h"

Pair*
mkpair(Val a, Val d)
{
	Pair *p;
	p = (Pair*)malq(Qpair);
	// setc*r would invoke write barrier
	_setcar(p, a);
	_setcdr(p, d);
	return p;
}

Pair*
mkweakpair(Val a, Val d)
{
	Pair *p;
	p = (Pair*)malweak();
	// setc*r would invoke write barrier
	_setcar(p, a);
	_setcdr(p, d);
	return p;
}
