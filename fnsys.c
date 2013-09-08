#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "x/include/lib9.h"

static void
l1_exit(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	int code;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to exit");
	code = 0;
	if(Vkind(argv[0]) == Qcval){
		cv = valcval(argv[0]);
		code = (int)cvalu(cv);
	}
	exit(code);
}

static void
l1_fork(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int pid;
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to fork");
	pid = fork();
	if(0 > pid)
		vmerr(vm, "fork: %s", strerror(errno));
	*rv = mkvallitcval(Vulong, pid);
}

static void
l1_waitpid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int pid, stat, opt, r;
	Cval *cv;
	List *l;
	
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to waitpid");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	cv = valcval(argv[0]);
	pid = (int)cvalu(cv);
	cv = valcval(argv[1]);
	opt = (int)cvalu(cv);
	r = waitpid(pid, &stat, opt);
	if(0 > r)
		vmerr(vm, "waitpid: %s", strerror(errno));
	l = mklist();
	listappend(vm, l, mkvallitcval(Vint, r));
	listappend(vm, l, mkvallitcval(Vint, stat));
	*rv = mkvallist(l);
}

extern char **environ;

static void
l1_environ(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	char **p;

	if(argc != 0)
		vmerr(vm, "wrong number of arguments to environ");
	p = environ;
	l = mklist();
	if(p)
		while(*p){
			_listappend(l, mkvalstr(mkstr0(*p)));
			p++;
		}
	*rv = mkvallist(l);
}

static void
l1_getenv(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *s, *t;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to getenv");
	checkarg(vm, argv, 0, Qstr);
	s = str2cstr(valstr(argv[0]));
	t = getenv(s);
	efree(s);
	if(t == 0)
		return;		/* nil */
	*rv = mkvalstr(mkstr0(t));
}

static void
l1_setenv(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *s, *t;
	int r;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to setenv");
	checkarg(vm, argv, 0, Qstr);
	checkarg(vm, argv, 1, Qstr);
	s = str2cstr(valstr(argv[0]));
	t = str2cstr(valstr(argv[1]));
	r = setenv(s, t, 1);
	efree(s);
	efree(t);
	if(0 > r)
		vmerr(vm, "setenv: %s", strerror(errno));
}

static void
l1_getpid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	*rv = mkvallitcval(Vulong, getpid());
}

static void
l1_gettimeofday(VM *vm, Imm argc, Val *argv, Val *rv)
{
	*rv = mkvallitcval(Vulong, usec());
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
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		days = cvalu(cv);
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
		cprintf(&l1stdout, "%s", p);

	efree(news);
	return;
badnews:
	xprintf("bad news\n");
	efree(news);
}

#if 0
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
	checkarg(vm, argv, 0, Qstr);
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
#endif


static void
l1_syscall(VM *vm, Imm argc, Val *argv, Val *rv)
{
	unsigned long i, xarg[6];
	Cval *cv;
	long xrv; // some systems return only an int.
	int sysn;
#if defined(__APPLE__) && defined(__MACH__)
	static long (*lsyscall)(int, ...) = (long (*)(int, ...))syscall;
        {
	static long (*syscall)(int, ...) = NULL;
        if(!syscall) syscall=lsyscall;
#endif

	if(argc < 1)
		vmerr(vm, "too few arguments to syscall");
	if(argc > 7)
		vmerr(vm, "too many arguments to syscall");
	checkarg(vm, argv, 0, Qcval);
	cv = valcval(argv[0]);
	sysn = (int)cvalu(cv);
	for(i = 1; i < argc; i++){
		checkarg(vm, argv, i, Qcval);
		cv = valcval(argv[i]);
		xarg[i-1] = (unsigned long)cvalu(cv);
	}
	setlasterrno(0);
	switch(argc){
	case 1:
		xrv = syscall(sysn);
		break;
	case 2:
		xrv = syscall(sysn, xarg[0]);
		break;
	case 3:
		xrv = syscall(sysn, xarg[0], xarg[1]);
		break;
	case 4:
		xrv = syscall(sysn, xarg[0], xarg[1], xarg[2]);
		break;
	case 5:
		xrv = syscall(sysn, xarg[0], xarg[1], xarg[2], xarg[3]);
		break;
	case 6:
		xrv = syscall(sysn, xarg[0], xarg[1], xarg[2], xarg[3],
			      xarg[4]);
		break;
	case 7:
		xrv = syscall(sysn, xarg[0], xarg[1], xarg[2], xarg[3],
			      xarg[4], xarg[5]);
		break;
	}
#if defined(__APPLE__) && defined(__MACH__)
        }
#endif
	setlasterrno(errno);
	*rv = mkvallitcval(Vlong, (Imm)xrv);
}

static void
l1_uname(VM *vm, Imm argc, Val *argv, Val *rv)
{
	struct utsname uts;
	List *l;

	if(argc != 0)
		vmerr(vm, "wrong number of arguments to uname");
	if(0 > uname(&uts))
		vmerr(vm, "uname: %s", strerror(errno));
	l = mklist();
	listappend(vm, l, mkvalstr(mkstr0(uts.sysname)));
	listappend(vm, l, mkvalstr(mkstr0(uts.release)));
	listappend(vm, l, mkvalstr(mkstr0(uts.version)));
	listappend(vm, l, mkvalstr(mkstr0(uts.machine)));
	*rv = mkvallist(l);
}

static void
l1_chdir(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *d;
	int r;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to chdir");
	checkarg(vm, argv, 0, Qstr);
	d = str2cstr(valstr(argv[0]));
	r = chdir(d);
	efree(d);
	if(0 > r)
		vmerr(vm, "chdir: %s", strerror(errno));
}

static void
l1_cwd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *buf;
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to cwd");
	buf = getcwd(0, 0);
	if(!buf)
		vmerr(vm, "cwd: %s", strerror(errno));
	*rv = mkvalstr(mkstr0(buf));
	free(buf);
}

static void
l1_errno(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	int no;
	l = mklistinit(2, Xnil);
	*rv = mkvallist(l);
	no = getlasterrno();
	listset(l, 0, mkvalcval2(mklitcval(Vint, no)));
	if(no == 0)
		return;
	listset(l, 1, mkvalstr(mkstr0(strerror(errno))));
}

static void
l1_usleep(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to usleep");
	checkarg(vm, argv, 0, Qcval);
	usleep(cvalu(valcval(argv[0])));
}

void
fnsys(Env env)
{
	FN(chdir);
	FN(cwd);
	FN(environ);
	FN(errno);
	FN(exit);
	FN(fork);
	FN(getenv);
	FN(getpid);
	FN(gettimeofday);
	FN(news);
	FN(setenv);
	FN(syscall);
	FN(uname);
	FN(usleep);
	FN(waitpid);
}
