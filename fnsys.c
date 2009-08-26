#include "sys.h"
#include "util.h"
#include "syscqct.h"

enum {
	Maxtrace = 1000000,	/* Trace records while profiler engaged  */
	Maxdefer = 1000000,	/* Deferred closure and code garbage */
};

typedef
struct Trace {
	Imm pc;
	Closure *cl;
} Trace;

typedef
struct Profiler
{
	Trace *trace;
	u32 ntrace;
	Head **defer;
	u32 ndefer;
	Freeheadfn freecl, freecode;
} Profiler;

static Profiler *prof;
static void	finiprof();

static void
l1_getenv(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *s, *t;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to getenv");
	checkarg(vm, "getenv", argv, 0, Qstr);
	s = str2cstr(valstr(argv[0]));
	t = getenv(s);
	if(t == 0)
		return;		/* nil */
	*rv = mkvalstr(mkstr0(t));
}

static void
l1_getpid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	*rv = mkvallitcval(Vulong, getpid());
}

static void
l1_gettimeofday(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Imm tod;
	struct timeval tv;

	gettimeofday(&tv, 0);
	tod = tv.tv_sec;
	tod *= 1000000;
	tod += tv.tv_usec;
	*rv = mkvallitcval(Vulong, tod);
}

static void
l1_randseed(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val arg0;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to randseed");
	arg0 = argv[0];
	if(arg0->qkind != Qcval)
		vmerr(vm, "operand 1 to randseed must be an integer");

	cv = valcval(arg0);
	srand((unsigned int)cv->val);
}

static void
l1_rand(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val arg0;
	Imm r;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rand");
	arg0 = argv[0];
	if(arg0->qkind != Qcval)
		vmerr(vm, "operand 1 to randseed must be an integer");

	cv = valcval(arg0);
	if(cv->val > RAND_MAX)
		vmerr(vm, "operand to rand exceeds RAND_MAX (%d)", RAND_MAX);
	if(cv->val == 0)
		vmerr(vm, "operand to rand must be positive");
	
	r = rand();
	r %= cv->val;
	*rv = mkvallitcval(Vulong, r);
}

static void
profsample(VM *vm)
{
	Trace *tr;
	if(prof->ntrace >= Maxtrace){
		xprintf("profile: trace buffer exhausted, "
			"profile suspended\n");
		setproftimer(0, 0);
		return;
	}
	tr = &prof->trace[prof->ntrace++];
	tr->pc = vm->pc;
	/* we assume that pc is on next insn, but it may not be if profiler
	   interrupted the insn fetch!  we catch only the easy, necessary
	   case of pc==0 */
	if(tr->pc != 0)
		tr->pc -= 1;
	tr->cl = vm->clx;
}

static Tab*
dotrs(VM *vm)
{
	char buf[256];
	Imm pc;
	Closure *cl;
	Code *code;
	Val k, v;
	Cval *cv;
	Trace *tr;
	Ctl **ls;
	Tab *tab;
	char *fn;
	
	tab = mktab();
	tr = prof->trace;
	while(tr < prof->trace+prof->ntrace){
		pc = tr->pc;
		cl = tr->cl;
		tr++;
		code = cl->code;
		if(cl->cfn || cl->ccl)
			snprint(buf, sizeof(buf), "%30s\t(builtin %s)",
				 cl->id, cl->cfn ? "function" : "closure");
		else{
			while(1){
				ls = code->labels;
				if(ls[pc] && ls[pc]->src){
					fn = (ls[pc]->src->filename
					      ? ls[pc]->src->filename
					      : "<stdin>");
					snprint(buf, sizeof(buf),
						 "%30s\t(%s:%u)", cl->id,
						 fn, ls[pc]->src->line);
					break;
				}
				if(pc == 0){
					snprint(buf, sizeof(buf),
						 "%30s", "(unknown code)");
					break;
				}
				pc--;
			}
		}
		k = mkvalstr(mkstr0(buf));
		v = tabget(tab, k);
		if(v){
			cv = valcval(v);
			v = mkvallitcval(Vuvlong, 1+cv->val);
		}else
			v = mkvallitcval(Vuvlong, 1);
		tabput(vm, tab, k, v);
	}
	xprintf("profile: %d samples\n", prof->ntrace);
	return tab;
}

static void
doprof()
{
	VM **vmp, *vm;

	vmp = vms;
	while(vmp < vms+Maxvms){
		vm = *vmp++;
		if(vm == 0)
			continue;
		profsample(vm);
	}
}

static int
deferfree(Head *hd)
{
	if(prof->ndefer >= Maxdefer-1){
		prof->defer[prof->ndefer++] = hd;
		setproftimer(0, 0);
		finiprof();
		xprintf("profile: deferral buffer exhausted, "
			"profile aborted\n");
		return 0;
	}
	prof->defer[prof->ndefer++] = hd;
	return 0;
}

static void
initprof()
{
	prof = emalloc(sizeof(Profiler));
	prof->trace = emalloc(Maxtrace*sizeof(Trace));
	prof->defer = emalloc(Maxdefer*sizeof(Head*));
	prof->freecl = getfreeheadfn(Qcl);
	prof->freecode = getfreeheadfn(Qcode);
	setfreeheadfn(Qcl, deferfree);
	setfreeheadfn(Qcode, deferfree);
}

static void
finiprof()
{
	Head *hd;

	while(prof->ndefer-- > 0){
		hd = prof->defer[prof->ndefer];
		switch(hd->qkind){
		case Qcl:
			prof->freecl(hd);
			break;
		case Qcode:
			prof->freecode(hd);
			break;
		default:
			fatal("unexpected object in deferral buffer");
		}
		heapfree(hd);
	}
	setfreeheadfn(Qcl, prof->freecl);
	setfreeheadfn(Qcode, prof->freecode);
	efree(prof->trace);
	efree(prof->defer);
	efree(prof);
	prof = 0;
}

static void
l1_profon(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to profon");
	if(prof)
		return;
	initprof();
	setproftimer(1000, doprof);
}

static void
l1_profoff(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to profoff");
	if(prof == 0)
		return;
	setproftimer(0, 0);
	*rv = mkvaltab(dotrs(vm));
	finiprof();
}

static void
l1_insncnt(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to insncnt");
	*rv = mkvaltab(doinsncnt(vm));
}

static void
l1_news(VM *vm, Imm argc, Val *argv, Val *rv)
{
	static char newspath[] = "../doc/news";
	static char newstime[] = ".l1news";
	static char *month[] = { "jan", "feb", "mar", "apr", "may",
				 "jun", "jul", "aug", "sep", "oct",
				 "nov", "dec" };
	char *news, *enews, **lp, *full, *p;
	unsigned len;
	Cval *cv;
	Imm days;
	int i, fd;
	char ntpath[128];
	struct stat st;
	struct tm tm;
	time_t lim, t;

	if(argc != 0 && argc != 1)
		vmerr(vm, "wrong number of arguments to news");
	if(argc == 1){
		checkarg(vm, "news", argv, 0, Qcval);
		cv = valcval(argv[0]);
		days = cv->val;
		lim = time(0)-(time_t)days*24*60*60;
	}else{
		snprint(ntpath, sizeof(ntpath), "%s/%s",
			getenv("HOME"), newstime);
		if(0 > stat(ntpath, &st))
			lim = 0;
		else
			lim = st.st_mtime;
		fd = open(ntpath, O_TRUNC|O_CREAT, 0644);
		if(fd >= 0)
			close(fd);
	}

	/* search vicinity of loadpath for news */
	lp = cqctloadpath;
	news = 0;
	while(*lp){
		len = strlen(*lp)+1+strlen(newspath)+1;
		full = emalloc(len);
		snprint(full, len, "%s/%s", *lp, newspath);
		news = readfile(full);
		efree(full);
		if(news)
			break;
		lp++;
	}
	if(news == 0)
		vmerr(vm, "cannot find the news");

	enews = news+strlen(news);
	p = news;

	/* find first news item to print */
	while(p && p < enews){
		if(!isspace(*p)){
			/* must be a date, the start of a new item */
			if(enews-p < 3)
				goto badnews;
			for(i = 0; i < 3; i++)
				p[i] = tolower(p[i]);
			for(i = 0; i < 12; i++)
				if(!strncmp(p, month[i], 3))
					break;
			if(i >= 12)
				goto badnews;
			memset(&tm, 0, sizeof(tm));
			tm.tm_sec = 59;
			tm.tm_min = 59;
			tm.tm_hour = 23;
			tm.tm_mon = i;
			if(2 != sscanf(p+3, "%d %d", &tm.tm_mday, &tm.tm_year))
				goto badnews;
			tm.tm_year -= 1900;
			t = mktime(&tm);
			if(t >= lim)
				break;
		}
		p = strchr(p, '\n');
		p++;
	}

	/* print news */
	if(p)
		cprintf(&vm->top->out, "%s", p);

	efree(news);
	return;
badnews:
	xprintf("bad news\n");
	efree(news);
}

static void
l1_glob(VM *vm, Imm argc, Val *argv, Val *rv)
{
	glob_t g;
	char *pat;
	int flags, r;
	size_t m;
	List *l;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to glob");
	checkarg(vm, "glob", argv, 0, Qstr);
	pat = str2cstr(valstr(argv[0]));
	/* see GLOB_ALTDIRFUNC (osx & linux) */
	flags = GLOB_TILDE|GLOB_BRACE;
	r = glob(pat, flags, 0, &g);
	efree(pat);
	if(r && r != GLOB_NOMATCH){
		globfree(&g);
		vmerr(vm, "glob failed");
	}
	l = mklist();
	for(m = 0; m < g.gl_pathc; m++)
		_listappend(l, mkvalstr(mkstr0(g.gl_pathv[m])));
	globfree(&g);
	*rv = mkvallist(l);
}

void
fnsys(Env *env)
{
	FN(getenv);
	FN(getpid);
	FN(gettimeofday);
	FN(glob);
	FN(insncnt);
	FN(news);
	FN(profoff);
	FN(profon);
	FN(rand);
	FN(randseed);
}
