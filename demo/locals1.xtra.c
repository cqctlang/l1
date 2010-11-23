#include <stdio.h>
#include <stdlib.h>
#include "locals1.h"

static struct Q q;
static Z z;

void
foo()
{
	z.bitfield1 = 0;
	q.x = 0;
}
