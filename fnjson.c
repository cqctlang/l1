#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
l1_json2val(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *s, *p, *e, c, *buf, *beg;
	static char *beg0 = "{ ";
	static char *begt = "{ @local true; true = 1; ";
	static char *begf = "{ @local false; false = 0; ";
	static char *begtf = "{ @local true, false; true = 1; false = 0; ";
	static char *end = "; }";
	Imm m;
	int t, f;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to json2val");
	checkarg(vm, argv, 0, Qstr);

	s = str2cstr(valstr(argv[0]));
	p = s;
	e = p+strlen(s);
	while(p < e){
		c = *p;
		if(c == '{')
			*p = '[';
	        if(c == '}')
			*p = ']';
		p++;
	}
	
	t = strstr(s, "true") ? 1 : 0;
	f = strstr(s, "false") ? 1 : 0;
	if(t && f)
		beg = begtf;
	else if(t)
		beg = begt;
	else if(f)
		beg = begf;
	else
		beg = beg0;

	m = strlen(beg)+strlen(s)+strlen(end)+1;
	buf = emalloc(m);
	snprint(buf, m, "%s%s%s", beg, s, end);
	cqcteval(vm, buf, "<json2val>", rv);
	efree(buf);
	efree(s);
}

void
fnjson(Env *env)
{
	FN(json2val);
}
