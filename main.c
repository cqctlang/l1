#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <inttypes.h> /* needed by cqct.h */
#include "cqct.h"

enum
{
	Maxloadpath = 255,
};

static unsigned char opt[256];
static char *argv0;

static void
usage()
{
	fprintf(stderr, "usage: %s [flags] [ <script> [ arg ... ] ]\n", argv0);
	fprintf(stderr, "\t-h print this usage\n");
	fprintf(stderr, "\t-l <dir> add <dir> to the load path\n");
	fprintf(stderr, "\t-m <file> initialize heap from <file>\n");
	fprintf(stderr, "\t-s suppress default load path\n");
	exit(0);
}

static void
fatal(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
	exit(1);
}

static void*
xmalloc(size_t size)
{
	void *p;
	p = malloc(size);
	if(!p)
		fatal("out of memory");
	memset(p, 0, size);
	return p;
}

static char*
xstrdup(char *s)
{
	void *p;
	if(s == 0)
		return 0;
	p = xmalloc(strlen(s)+1);
	memcpy(p, s, strlen(s));
	return p;
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
		try = xmalloc(q-p+1+nl+1);
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

	/* on linux, let the system tell us where the executable is */
	if(access("/proc/self/exe", R_OK) == 0){
		buf = xmalloc(MAXPATHLEN+1);
		if(readlink("/proc/self/exe", buf, MAXPATHLEN))
			return buf;
		free(buf);
	}

	/* on non-linux, and on broken linux, attempt to find it */
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
		buf = xmalloc(MAXPATHLEN+1+strlen(path)+1);
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
		buf = xmalloc(MAXPATHLEN+1+strlen(p)+1);
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
	if(path != buf)
		free(buf);
	free(tmp);
	return 0;
}

static unsigned
finishloadpath(char **lp, unsigned nlp)
{
	char *p, *root;

	if(nlp >= Maxloadpath)
		fatal("too many elements in load path");
	lp[nlp++] = xstrdup(".");
	if(opt['s']){
		/* no system load path */
		lp[nlp] = 0;
		return nlp;
	}

	/* the various system load path policies */
#ifdef LIBDIR
	s = xstrdup(LIBDIR);
	p = strtok(p, ":");
	while(p){
		if(nlp >= Maxloadpath)
			fatal("too many elements in load path");
		lp[nlp++] = xstrdup(p);
		p = strtok(NULL, ":");
	}
	free(s);
#endif
	p = getenv("L1LIBPATH");
	if(p){
		p = strtok(p, ":");
		while(p){
			if(nlp >= Maxloadpath)
				fatal("too many elements in load path");
			lp[nlp++] = xstrdup(p);
			p = strtok(NULL, ":");
		}
	}else{
#ifndef LIBDIR
		argv0 = readlinkf(argv0);
		if(argv0 == 0)
			fatal("cannot locate l1 executable");
		if(nlp >= Maxloadpath)
			fatal("too many elements in load path");
		root = dirname(argv0);
		lp[nlp] = xmalloc(strlen(root)+1+4+1);
		sprintf(lp[nlp++], "%s/lib", root);
		free(argv0);
#endif
	}

	lp[nlp] = 0;
	return nlp;
}

int
main(int argc, char *argv[])
{
	char *lp[Maxloadpath+1]; /* last one for null */
	unsigned nlp;
	char *memfile;
	int i, rv;
	VM *vm;
	Val fn, *xargv, v;

	memset(lp, 0, sizeof(lp));
	nlp = 0;
	memfile = 0;
	argv0 = argv[0];

	i = 1;
	while(i < argc){
		if(strcmp("-l", argv[i]) == 0){
			if(i+1 >= argc)
				usage();
			if(nlp >= Maxloadpath)
				fatal("too many elements in load path");
			lp[nlp++] = xstrdup(argv[i+1]);
			memmove(argv+i, argv+i+2, (argc-i-2)*sizeof(char*));
			argc -= 2;
		}else if(strcmp("-m", argv[i]) == 0){
			if(i+1 >= argc)
				usage();
			if(opt['m'])
				usage();
			opt['m'] = 1;
			memfile = xstrdup(argv[i+1]);
			memmove(argv+i, argv+i+2, (argc-i-2)*sizeof(char*));
			argc -= 2;
		}else if(strcmp("-s", argv[i]) == 0){
			opt['s'] = 1;
			memmove(argv+i, argv+i+1, (argc-i-1)*sizeof(char*));
			argc--;
		}else{
			/* pass through legacy flags referenced by run time */
			if(argv[i][0] == '-' && strlen(argv[i]) == 2)
				cqctflags[(int)argv[i][1]] = 1;
			i++;
		}
	}

	nlp = finishloadpath(lp, nlp);
	vm = cqctinit(memfile, lp);
	if(vm == 0)
		exit(1);
	fn = cqctenvlook(vm, "$l1start");
	if(fn == 0)
		fatal("$l1start is not defined");
	xargv = xmalloc(argc*sizeof(Val));
	for(i = 0; i < argc; i++)
		xargv[i] = cqctcstrval(argv[i]);
	rv = 0;
	cqctcallfn(vm, fn, argc, xargv, &v);
	free(xargv);
	if(Vkind(v) == Qcval)
		rv = cqctvalint32(v);
	cqctfini(vm);
	return rv;
}
