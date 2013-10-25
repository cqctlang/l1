#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

typedef unsigned long long uptr;
static char opt[256];

#define roundup(l,n)   ((uptr)(((uptr)(l)+((n)-1))&~((n)-1)))

static void
usage(char *argv0)
{
	printf("usage: %s -a <alignment> -s <symbol> -m <arch> [ -l <maxlen> ] <infile> <outfile>\n",
	       argv0);
	exit(1);
}

int
main(int argc, char *argv[])
{
	int ifd, ofd;
	int c;
	struct stat st;
	uptr align, len, maxlen, mlen;
	char *sym;
	void *data;
	char *ep;
	char *mach;
	char *infile, *outfile;

	maxlen = 0;
	align = 0;
	len = 0;
	sym = 0;
	mach = 0;
	while(-1 != (c = getopt(argc, argv, "a:hl:m:s:")))
		switch(c) {
		case 'a':
			opt['a'] = 1;
			align = strtoull(optarg, &ep, 0);
			if(*ep != '\0')
				usage(argv[0]);
			break;
		case 's':
			opt['s'] = 1;
			sym = optarg;
			break;
		case 'l':
			opt['l'] = 1;
			maxlen = strtoull(optarg, &ep, 0);
			if(*ep != '\0')
				usage(argv[0]);
			break;
		case 'm':
			opt['m'] = 1;
			mach = optarg;
			break;
		case 'h':
		case '?':
			usage(argv[0]);
			break;
		}

	if(opt['a'] == 0 || opt['s'] == 0 || opt['m'] == 0)
		usage(argv[0]);

	if(argc-optind != 2)
		usage(argv[0]);

	infile = argv[optind];
	outfile = argv[optind+1];

	ifd = open(infile, O_RDONLY);
	if(0 > ifd) {
		fprintf(stderr, "open: %s: %s\n", infile, strerror(errno));
		exit(1);
	}
	if(0 > fstat(ifd, &st)) {
		fprintf(stderr, "stat: %s: %s\n", infile, strerror(errno));
		exit(1);
	}
	len = st.st_size;
	if(opt['l'] && maxlen < len)
		len = maxlen;
	mlen = roundup(len, 4096);
	if(mlen == 0)
		mlen = 4096;
	data = mmap(0, mlen, PROT_READ, MAP_PRIVATE, ifd, 0);
	if(data == MAP_FAILED) {
		fprintf(stderr, "mmap: %s: %s\n", infile, strerror(errno));
		exit(1);
	}
	close(ifd);
	ofd = open(outfile, O_TRUNC|O_CREAT|O_WRONLY, 0664);
	if(0 > ofd) {
		fprintf(stderr, "open: %s: %s\n", outfile, strerror(errno));
		exit(1);
	}

	{
		int x;
		FILE *of;

		of=fdopen(ofd,"w");
		fprintf(of,"unsigned char %s[]={\n",sym);

		for(x=0;x<mlen;x++) {
			if(!(x%16)) {
				fprintf(of,"\n");
			}
			fprintf(of,"0x%02x,",*(((unsigned char *)data) + x));
		}

		fprintf(of,"\n};\n");
		//fprintf(of,"unsigned char endheapimage[]={0x00};\n");
		fprintf(of,"unsigned long long len%s=%d;\n",sym,(int)mlen);
		fflush(of);
	}

	munmap(data, mlen);
	close(ofd);
	return 0;
}
