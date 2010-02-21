#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/param.h>
#include <libgen.h>

#include "cqct.h"

enum{
	Maxloadpath = 128,
};

static VM *vm;

static void
sigint(int sig)
{
	cqctinterrupt(vm);
}

static void
usage(char *argv0)
{
	fprintf(stderr, "usage: %s [flags] [ <script> [ arg ... ] ]\n",
		argv0);
	fprintf(stderr, "unless <script> is provided, "
		"runs in interactive evaluation mode:\n");
	fprintf(stderr, "\ttype cinquecento expression on stdin, "
		"followed by newline;\n");
	fprintf(stderr, "\tto exit send EOF (^d).\n");
	fprintf(stderr, "\nuser flags:\n");
	fprintf(stderr, "\t-h print this usage\n");
	fprintf(stderr, "\t-r allow redefinition of implicitly called builtins\n");
	fprintf(stderr, "\t-m <N> limit heap to <N> megabytes\n");
	fprintf(stderr, "\t-t report timing statistics\n");
	fprintf(stderr, "\t-w print warnings about dodgy code\n");
	fprintf(stderr, "\t-z send output to /dev/null\n");
	fprintf(stderr, "\nl1 internals flags:\n");
	fprintf(stderr, "\t-o dump disassembled object code\n");
	fprintf(stderr, "\t-p dump IR at various stages\n");
	fprintf(stderr, "\t-q dump expanded cinquecento source\n");
	fprintf(stderr, "\t-b dump frame storage\n");
	fprintf(stderr, "\t-c do not compile expanded source\n");
	fprintf(stderr, "\t-x do not execute object code\n");
	fprintf(stderr, "\t-g do not run gc in separate thread\n");

	exit(0);
}

static void
fatal(char *fmt, ...)
{
	va_list args;
	vprintf(fmt, args);
	printf("\n");
	abort();
}

static void*
emalloc(size_t size)
{
	void *p;
	p = malloc(size);
	if(!p)
		fatal("out of memory");
	memset(p, 0, size);
	return p;
}

static void*
erealloc(void *p, size_t old, size_t new)
{
	p = realloc(p, new);
	if(p == 0)
		fatal("out of memory");
	if(new > old)
		memset(p+old, 0, new-old);
	return p;
}

static char*
xstrdup(char *s)
{
	void *p;
	if(s == 0)
		return 0;
	p = emalloc(strlen(s)+1);
	memcpy(p, s, strlen(s));
	return p;
}

static char*
readexpr(char *prompt)
{
	char *buf;
	unsigned len, m, rv, cnt;

	printf("%s", prompt);
	len = 1024;
	buf = emalloc(len);
	m = len-1;
	cnt = 0;
	while(1){
		if(m == 0){
			buf = erealloc(buf, len, 2*len);
			len *= 2;
			m = len-1;
		}
		if(0 == fgets(buf+cnt, len, stdin)){
			free(buf);
			return 0;
		}
		rv = strlen(buf);
		if(buf[rv-1] == '\n')
			return buf;
		m -= rv;
		cnt += rv;
	}
}

static char*
readfd(int fd)
{
	char *buf, *p;
	int rv;
	unsigned m, max, off;

	max = 1024;
	buf = emalloc(max);
	p = buf;
	m = max-1;
	while(1){
		if(m == 0){
			off = p-buf;
			buf = erealloc(buf, max, 2*max);
			p = buf+off;
			max *= 2;
			m = max-off;
		}
		rv = read(fd, p, m);
		if(0 > rv){
			if(errno == EINTR || EAGAIN)
				continue;
			free(buf);
			return 0;
		}
		if(0 == rv)
			return buf;
		m -= rv;
		p += rv;
	}
}

static char*
readfile(char *filename)
{
	int fd;
	char *buf;
	struct stat st;

	if(0 > stat(filename, &st))
		return 0;
	if(!S_ISREG(st.st_mode)){
		errno = EISDIR;
		return 0;
	}
	fd = open(filename, O_RDONLY);
	if(0 > fd)
		return 0;
	buf = readfd(fd);
	close(fd);
	return buf;
}

/*
 * In place, rewrite name to compress multiple /, eliminate ., and process ..
 * Lifted from plan9ports.
 */
#define SEP(x)	((x)=='/' || (x) == 0)
char*
cleanname(char *name)
{
	char *p, *q, *dotdot;
	int rooted;

	rooted = name[0] == '/';

	/*
	 * invariants:
	 *	p points at beginning of path element we're considering.
	 *	q points just past the last path element we wrote (no slash).
	 *	dotdot points just past the point where .. cannot backtrack
	 *		any further (no slash).
	 */
	p = q = dotdot = name+rooted;
	while(*p) {
		if(p[0] == '/')	/* null element */
			p++;
		else if(p[0] == '.' && SEP(p[1]))
			p += 1;	/* don't count the separator in case it is nul */
		else if(p[0] == '.' && p[1] == '.' && SEP(p[2])) {
			p += 2;
			if(q > dotdot) {	/* can backtrack */
				while(--q > dotdot && *q != '/')
					;
			} else if(!rooted) {	/* /.. is / but ./../ is .. */
				if(q != name)
					*q++ = '/';
				*q++ = '.';
				*q++ = '.';
				dotdot = q;
			}
		} else {	/* real path element */
			if(q != name+rooted)
				*q++ = '/';
			while((*q = *p) != '/' && *q != 0)
				p++, q++;
		}
	}
	if(q == name)	/* empty string is really ``.'' */
		*q++ = '.';
	*q = '\0';
	return name;
}

static char*
searchpath(char *name)
{
	char *p, *np, *q, *w, *try;
	unsigned nl;

	nl = strlen(name);
	p = getenv("PATH");
	if(p == 0)
		return 0;
	q = p;
	while(*p){
		q = strchr(p, ':');
		if(q == 0){
			q = p+strlen(p); /* last element */
			np = q;
		}else
			np = q+1;
		try = emalloc(q-p+1+nl+1);
		memcpy(try, p, q-p);
		w = try+(q-p);
		if(w[-1] != '/')
			*w++ = '/';
		memcpy(w, name, nl);
		if(access(try, X_OK) == 0)
			return try;
		free(try);
		p = np;
	}
	return 0;
}

static char*
readlinkf(char *path)
{
	char *buf, *tmp, *p, *q;
	struct stat st;
	ssize_t sz, psz;

	buf = 0;
	tmp = 0;
	if(path[0] != '/' && path[0] != '.'){
		p = searchpath(path);
		if(p)
			path = p;
		else
			path = xstrdup(path);
	}else
		path = xstrdup(path);

	if(path[0] != '/'){
		buf = emalloc(MAXPATHLEN+1+strlen(path)+1);
		p = getcwd(buf, MAXPATHLEN);
		if(p == 0)
			goto fail;
		q = p+strlen(p);
		if(*q != '/')
			*q++ = '/';
		memcpy(q, path, strlen(path));
		free(path);
	}else
		p = buf = path;

	while(1){
		cleanname(p);
		if(0 > lstat(p, &st))
			goto fail;
		if((st.st_mode&S_IFLNK) != S_IFLNK)
			break;
		tmp = buf;
		buf = emalloc(MAXPATHLEN+1+strlen(p)+1);
		sz = readlink(p, buf, MAXPATHLEN);
		if(sz == -1)
			goto fail;
		if(buf[0] != '/'){
			/* replace file name with symlink target */
			psz = strlen(p);
			while(p[psz-1] == '/')
				p[--psz] = '0';
			q = strrchr(p, '/');
			if(*q == 0)
				goto fail;
			q++;
			psz = q-p;
			memmove(buf+psz, buf, sz);
			memcpy(buf, p, psz);
		}
		p = buf;
		free(tmp); tmp = 0;
	}

	return p;
fail:
	free(path);
	free(buf);
	free(tmp);
	return 0;
}

static void
tvdiff(struct timeval *a, struct timeval *b, struct timeval *c)
{
        c->tv_sec = a->tv_sec - b->tv_sec;
        c->tv_usec = a->tv_usec - b->tv_usec;
        if (c->tv_usec < 0) {
                c->tv_sec -= 1;
                c->tv_usec += 1000000;
        }
}

struct memusage
{
	uint64_t size;
	uint64_t rss;
};

// FIXME: we assume linux but fail graceless if not;
// how do you get vm size on os x?
static int
memusage(struct memusage *mu)
{
	int fd;
	char buf[128];

	memset(mu, 0, sizeof(*mu));
	fd = open("/proc/self/statm", O_RDONLY);
	if(0 > fd)
		return -1;
	memset(buf, 0, sizeof(buf));
	read(fd, buf, sizeof(buf)-1);
	close(fd);
	if(2 != sscanf(buf, "%" PRIu64 " %" PRIu64, &mu->size, &mu->rss))
		return -1;
	return 0;
}

static uint64_t
rdtsc()
{
	uint32_t hi, lo;
	asm("rdtsc" : "=a"(lo), "=d"(hi));
	return (uint64_t)lo|((uint64_t)hi<<32);
}


int
main(int argc, char *argv[])
{
	Val entry, fn, v;
	char *filename;
	int c;
	struct timeval beg, end;
	uint64_t bt, et;
	int dorepl;
	char opt[256];
	char *inbuf, *s;
	uint64_t heapmax;
	int i, valc;
	Val *valv;
	char *argv0, *root;
	unsigned n, nlp;
	char *lp[Maxloadpath+1];	/* extra one is final null */
	Toplevel *top;
	char *ename, *argsid;
	struct memusage mu;
	uint64_t usec;
	int rv;
	Xfd *xfd, devnull;
	int status;

	argv0 = argv[0];
	memset(opt, 0, sizeof(opt));
	opt['x'] = 1;		/* execute */
	opt['g'] = 1;		/* gc in separate thread */
	opt['s'] = 1;		/* include default load path */
	dorepl = 1;
	ename = 0;
	heapmax = 0;
	nlp = 0;
	filename = 0;
	while(EOF != (c = getopt(argc, argv, "a+be:ghkl:m:opqrstTwxz"))){
		switch(c){
		case 'a':
		case 'b':
		case 'k':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 'w':
		case 'T':
			cqctflags[c] = 1;
			break;
		case 't':
		case 'z':
			opt[c] = 1;
			break;
		case 'g':
		case 's':
		case 'x':
			opt[c] = 0;
			break;
		case 'e':
			ename = optarg;
			break;
		case 'm':
			heapmax = atoi(optarg);
			break;
		case 'l':
			if(nlp >= Maxloadpath)
				fatal("too many directories in load path");
			n = strlen(optarg);
			while(n > 0 && optarg[n-1] == '/')
				n--;
			lp[nlp] = emalloc(n+1);
			memcpy(lp[nlp++], optarg, n);
			break;
		case 'h':
		case '?':
			usage(argv0);
		}
	}

	if(argv[optind]){
		filename = argv[optind++];
		dorepl = 0;
	}
	if(ename && dorepl)
		fatal("-e requires a script");

	argv0 = readlinkf(argv0);
	if(argv0 == 0)
		fatal("cannot locate l1 executable");

	if(opt['s']){
		if(nlp >= Maxloadpath)
			fatal("too many directories in load path");
		root = dirname(argv0);
		lp[nlp] = emalloc(strlen(root)+1+4+1);
		sprintf(lp[nlp++], "%s/lib", root);
	}
	free(argv0);
	lp[nlp] = 0;

	xfd = 0;
	if(opt['z']){
		/* suppress output */
		memset(&devnull, 0, sizeof(devnull));
		xfd = &devnull;
	}

	top = cqctinit(opt['g'], heapmax, lp, 0, xfd, 0);
	while(nlp > 0)
		free(lp[--nlp]);
	if(opt['x']){
		vm = cqctmkvm(top);
		if(vm == 0){
			cqctfini(top);
			return -1;
		}
		signal(SIGINT, sigint);
	}

	if(dorepl){
		if(setvbuf(stdin, 0, _IONBF, 0))
			fatal("cannot clear stdin buffering");
	}

	status = 0;
	valc = 0;
	valv = 0;
	argsid = 0;
	if(!dorepl){
		valc = argc-optind+1;
		valv = emalloc(valc*sizeof(Val));
		valv[0] = cqctcstrval(argv[0]);
		for(i = 1; i < valc; i++)
			valv[i] = cqctcstrval(argv[optind+i-1]);
		argsid = "args";
	}

	do{
		inbuf = 0;
		if(dorepl){
			inbuf = readexpr("; ");
			if(inbuf == 0){
				printf("\n");
				break;
			}
		}else{
			if(!strcmp(filename, "-")){
				filename = "<stdin>";
				inbuf = readfd(0);
			}else
				inbuf = readfile(filename);
			if(inbuf == 0){
				printf("%s: %s\n", filename, strerror(errno));
				continue;
			}
		}

		if(opt['t']){
			gettimeofday(&beg, 0);
			bt = rdtsc();
		}
		entry = cqctcompile(inbuf, filename, top, ename ? 0 : argsid);
		free(inbuf);
		if(entry == 0)
			continue;
		if(opt['x'] == 0)
			continue; /* just compiling */
		if(!ename)
			rv = cqctcallfn(vm, entry, valc, valv, &v);
		else{
			rv = cqctcallfn(vm, entry, 0, 0, &v);
			if(rv)
				continue; /* error */
			fn = cqctenvlook(top, ename);
			if(fn == 0)
				fatal("entry %s is undefined");
			rv = cqctcallfn(vm, fn, valc, valv, &v);
		}
		if(opt['t']){
			et = rdtsc();
			gettimeofday(&end, 0);
			tvdiff(&end, &beg, &end);
			usec = 1000000*end.tv_sec+end.tv_usec;
			if(dorepl){
				printf("%" PRIu64 " usec", usec);
				printf("\t%" PRIu64 " cycles", et-bt);
				if(0 == memusage(&mu))
					printf("\t%10" PRIu64 "K vm  "
					       "%10" PRIu64 "K rss",
					       4*mu.size, 4*mu.rss);
				printf("\n");
			}
		}
		if(rv)
			continue; /* error */
		if(dorepl && Vkind(v) != Qnil){
			s = cqctsprintval(vm, v);
			printf("%s\n", s);
			cqctfreecstr(s);
		}
		status = 0;
		if(Vkind(v) == Qcval)
			status = cqctvalint32(v);

	}while(dorepl);

	free(valv);
	if(opt['x'])
		cqctfreevm(vm);
	cqctfini(top);

	if(opt['t'] && !dorepl){
		printf("%" PRIu64 " usec", usec);
		printf("\t%" PRIu64 " cycles", et-bt);
		if(0 == memusage(&mu))
			printf("\t%10" PRIu64 "K vm  "
			       "%10" PRIu64 "K rss",
			       4*mu.size, 4*mu.rss);
		printf("\n");
	}

	return status;
}
