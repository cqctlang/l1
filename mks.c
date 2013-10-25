#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

typedef unsigned long long uptr;
static char opt[256];

static void
usage(char *argv0)
{
	printf("usage: %s -a <alignment> -s <symbol> [ -l <maxlen> ] <infile> <outfile>\n",
	       argv0);
	exit(1);
}

int
main(int argc, char *argv[])
{
	int ofd;
	int c;
	uptr align, len, maxlen;
	char *sym;
	char *ep;
	char *infile, *outfile;
	FILE *of;

	maxlen = 0;
	align = 0;
	len = 0;
	sym = 0;
	while(-1 != (c = getopt(argc, argv, "a:hl:s:")))
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
		case 'h':
		case '?':
			usage(argv[0]);
			break;
		}

	if(opt['a'] == 0 || opt['s'] == 0)
		usage(argv[0]);

	if(argc-optind != 2)
		usage(argv[0]);

	infile = argv[optind];
	outfile = argv[optind+1];

	ofd = open(outfile, O_TRUNC|O_CREAT|O_WRONLY, 0664);
	if(0 > ofd) {
		fprintf(stderr, "open: %s: %s\n", outfile, strerror(errno));
		exit(1);
	}

	of = fdopen(ofd, "w");
	if(!of) {
		fprintf(stderr, "fdopen: %s: %s\n", outfile, strerror(errno));
		exit(1);
	}

	fprintf(of, ".section savedheap, \"r\"\n");
	if(align)
		fprintf(of, ".balign 0x%x\n", (int)align);
	fprintf(of, ".globl\t_%s\n", sym);
	fprintf(of, ".globl\t_end%s\n", sym);
	fprintf(of, "_%s:\n", sym);
	fprintf(of, ".incbin \"%s\"\n", infile);
	fprintf(of, "_end%s:\n", sym);
	fflush(of);
	close(ofd);
	return 0;
}
