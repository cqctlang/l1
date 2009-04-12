#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <libgen.h>

#include "cqct.h"

enum{
	Maxloadpath = 128,
};

static void
usage(char *argv0)
{
	fprintf(stderr, "usage: %s [flags] [ <script> [ arg ... ] ]\n",
		argv0);
	fprintf(stderr, "without -e, runs in interactive evaluation mode:\n");
	fprintf(stderr, "\ttype cinquecento expressions on stdin, "
		" followed by newline\n");
	fprintf(stderr, "\tto exit send SIGTERM (^c).\n");
	fprintf(stderr, "\nuser flags:\n");
	fprintf(stderr, "\t-h print this usage\n");
	fprintf(stderr, "\t-t report timing statistics\n");
	fprintf(stderr, "\t-m <N> limit heap to <N> megabytes\n");
	fprintf(stderr, "\t-w print warnings about dodgy code\n"); 
	fprintf(stderr, "\t-z send output to /dev/null\n");
	fprintf(stderr, "\nl1 developer flags:\n");
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

static ssize_t
xread(int fd, char *p, size_t len)
{
	size_t nr;
	ssize_t rv;
	
	nr = 0;
	while(nr < len){
		rv = read(fd, p, len-nr);
		if(0 > rv && errno == EINTR)
			continue;
		if(0 > rv)
			return -1;
		if(0 == rv)
			return nr;
		nr += rv;
		p += rv;
	}
	return nr;
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
readfile(char *filename)
{
	char *buf;
	struct stat st;
	int fd;

	if(0 > stat(filename, &st))
		return 0;
	fd = open(filename, O_RDONLY);
	if(0 > fd)
		return 0;
	buf = emalloc(st.st_size+1);
	if(0 > xread(fd, buf, st.st_size)){
		free(buf);
		close(fd);
		return 0;
	}
	close(fd);
	return buf;
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

int
main(int argc, char *argv[])
{
	Closure *entry;
	Val v;
	Expr *e;
	VM *vm;
	char *filename;
	int c;
	struct timeval beg, end;
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
	char *argsid;
	struct memusage mu;
	uint64_t usec;
	int rv;

	argv0 = argv[0];
	memset(opt, 0, sizeof(opt));
	opt['x'] = 1;		/* execute */
	opt['g'] = 1;		/* gc in separate thread */
	opt['s'] = 1;		/* include default load path */
	dorepl = 1;
	heapmax = 0;
	nlp = 0;
	filename = 0;
	while(EOF != (c = getopt(argc, argv, "a+bhol:m:pqtwgxsz"))){
		switch(c){
		case 'a':
		case 'o':
		case 'p':
		case 'q':
		case 'b':
		case 'w':
			cqctflags[c] = 1;
			break;
		case 't':
		case 'z':
			opt[c] = 1;
			break;
		case 'x':
		case 'g':
		case 's':
			opt[c] = 0;
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

	if(opt['s']){
		if(nlp >= Maxloadpath)
			fatal("too many directories in load path");
		root = dirname(argv0);
		lp[nlp] = emalloc(strlen(root)+1+4+1);
		sprintf(lp[nlp++], "%s/lib", root);
	}
	lp[nlp] = 0;
	
	top = cqctinit(opt['g'], heapmax, lp);
	while(nlp > 0)
		free(lp[--nlp]);
	if(opt['x']){
		vm = cqctmkvm(top);
		if(vm == 0){
			cqctfini(top);
			return -1;
		}
		if(opt['z']){
			if(0 > cqcteval(vm,
					"stdout = open(\"/dev/null\", \"w\");",
					0, 0)) {
				cqctfini(top);
				return -1;
			}
		}
	}

	if(dorepl){
		if(setvbuf(stdin, 0, _IONBF, 0))
			fatal("cannot clear stdin buffering");
	}

	valc = 0;
	valv = 0;
	argsid = 0;
	if(!dorepl){
		valc = argc-optind;
		valv = emalloc(valc*sizeof(Val));
		for(i = 0; i < valc; i++)
			valv[i] = cqctcstrval(argv[optind+i]);
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
			inbuf = readfile(filename);
			if(inbuf == 0){
				printf("%s: %s\n", filename, strerror(errno));
				continue;
			}
		}
		e = cqctparsestr(inbuf, filename);
		free(inbuf);
		if(e == 0)
			continue;
		entry = cqctcompile(e, top, argsid);
		if(entry == 0){
			cqctfreeexpr(e);
			continue;
		}
		/* now storage for E is managed by cqct storage manager */

		if(opt['x'] == 0)
			continue; /* just compiling */
		
		if(opt['t'])
			gettimeofday(&beg, 0);
		rv = cqctcallfn(vm, entry, valc, valv, &v);
		if(opt['t']){
			gettimeofday(&end, 0);
			tvdiff(&end, &beg, &end);
			usec = 1000000*end.tv_sec+end.tv_usec;
			if(dorepl){
				if(0 > memusage(&mu))
					printf("%" PRIu64 " usec\n", usec);
				else
					printf("%10" PRIu64 " usec"
					       "%10" PRIu64 "K vm  "
					       "%10" PRIu64 "K rss\n",
					       usec, 4*mu.size, 4*mu.rss);
			}
		}
		if(rv)
			continue; /* error */
		if(dorepl && v->qkind != Qnil){
			s = cqctsprintval(vm, v);
			printf("%s\n", s);
			cqctfreecstr(s);
		}
	}while(dorepl);

	free(valv);
	if(opt['x'])
		cqctfreevm(vm);
	cqctfini(top);

	if(opt['t'] && !dorepl){
		if(0 > memusage(&mu))
			printf("%10" PRIu64 " usec\n", usec);
		else
			printf("%10" PRIu64 " usec"
			       "%10" PRIu64 "K vm  "
			       "%10" PRIu64 "K rss\n",
			       usec, 4*mu.size, 4*mu.rss);
	}

	return 0;
}
