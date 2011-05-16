#include "sys.h"
#include "util.h"
#include "syscqct.h"

Ctype*
fieldtype(Vec *s)
{
	return valctype(vecref(s, Typepos));
}

Ctype*
paramtype(Vec *s)
{
	return valctype(vecref(s, Typepos));
}

Ctype*
symtype(Vec *s)
{
	return valctype(vecref(s, Typepos));
}
