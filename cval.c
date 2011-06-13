#include "sys.h"
#include "util.h"
#include "syscqct.h"

u32
hashqcval(Cval *v)
{
	return hashx(hashu64(v->val), hashctype(v->type));
}

int
eqcval(Cval *a, Cval *b)
{
	if(a->val != b->val)
		return 0;
	return eqvctype(a->type, b->type);
}

u32
hashqvcval(Cval *v)
{
	return hashqcval(v);
}

int
eqvcval(Cval *a, Cval *b)
{
	return eqcval(a, b);
}

u32
hashcval(Cval *v)
{
	return hashu64(v->val);
}

int
equalcval(Cval *a, Cval *b)
{
	return a->val == b->val;
}
