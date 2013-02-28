#include <stdio.h>
#include <stdlib.h>
#include "elf.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

typedef unsigned long long uptr;
static char opt[256];

/* undef + .shstrtab, .data, .symtab, .strtab */
#define Nsect (1+4)
#define Maxstrtab 128
#define Maxsymtab 128
#define roundup(l,n)   ((uptr)(((uptr)(l)+((n)-1))&~((n)-1)))

static uptr
xwrite(int fd, void *p, uptr len)
{
	uptr ns;
	ssize_t rv;

	ns = 0;
	while(ns < len){
		rv = write(fd, p, len-ns);
		if(0 > rv)
			return -1;
		ns += rv;
		p += rv;
	}
	return ns;
}

typedef
struct Symtab
{
	Elf64_Sym tab[Maxsymtab];
	Elf64_Sym *p, *e;
} Symtab;

typedef
struct Strtab
{
	char tab[Maxstrtab];
	char *p, *e;
} Strtab;

static void
initsymtab(Symtab *st)
{
	memset(st, 0, sizeof(Symtab));
	st->p = st->tab;
	st->e = st->p+sizeof(st->tab)/sizeof(st->tab[0]);
}

static void
addsymtab(Symtab *st,
	  Elf64_Word name,
	  Elf64_Section shndx,
	  Elf64_Addr addr,
	  Elf64_Addr size)
{
	Elf64_Sym *s;
	if(st->p >= st->e){
		fprintf(stderr, "symbol table overflow\n");
		exit(1);
	}
	s = st->p++;
	s->st_name = name;
	s->st_info = ELF64_ST_INFO(STB_GLOBAL, STT_OBJECT);
	s->st_other = ELF64_ST_VISIBILITY(STV_DEFAULT);
	s->st_shndx = shndx;
	s->st_value = addr;
	s->st_size = size;
}

static unsigned int
symtablen(Symtab *st)
{
	return (st->p-st->tab)*sizeof(st->tab[0]);
}

static void
initstrtab(Strtab *st)
{
	memset(st, 0, sizeof(Strtab));
	st->p = st->tab+1;
	st->e = st->tab+sizeof(st->tab)/sizeof(st->tab[0]);
}

static unsigned int
addstrtab(Strtab *st, char *s)
{
	unsigned int len, idx;
	len = strlen(s)+1;
	if(st->p+len > st->e){
		fprintf(stderr, "string table overflow\n");
		exit(1);
	}
	memcpy(st->p, s, len);
	idx = st->p-st->tab;
	st->p += len;
	return idx;
}

static unsigned int
strtablen(Strtab *st)
{
	return (st->p-st->tab)*sizeof(st->tab[0]);
}

static void
mkelfo(int fd, char *sym, void *data, uptr len, uptr align)
{
	Elf64_Ehdr hdr;
	Elf64_Shdr shdr[Nsect];	
	Strtab shstrtab;
	Strtab symstrtab;
	Symtab symtab;
	uptr dataoff, shstrtaboff, symtaboff, symstrtaboff;
	char endsym[128];

	initstrtab(&shstrtab);
	initstrtab(&symstrtab);
	initsymtab(&symtab);

	memset(&hdr, 0, sizeof(hdr));
	hdr.e_ident[EI_MAG0] = ELFMAG0;
	hdr.e_ident[EI_MAG1] = ELFMAG1;
	hdr.e_ident[EI_MAG2] = ELFMAG2;
	hdr.e_ident[EI_MAG3] = ELFMAG3;
	hdr.e_ident[EI_CLASS] = ELFCLASS64;
	hdr.e_ident[EI_DATA] = ELFDATA2LSB;
	hdr.e_ident[EI_VERSION] = EV_CURRENT;
	hdr.e_ident[EI_OSABI] = ELFOSABI_SYSV;
	hdr.e_type = ET_REL;
	hdr.e_machine = EM_X86_64;
	hdr.e_version = EV_CURRENT;
	hdr.e_entry = 0;
	hdr.e_phoff = 0;
	hdr.e_shoff = sizeof(Elf64_Ehdr);
	hdr.e_flags = 0;
	hdr.e_ehsize = sizeof(Elf64_Ehdr);
	hdr.e_phentsize = 0;
	hdr.e_phnum = 0;
	hdr.e_shentsize = sizeof(Elf64_Shdr);
	hdr.e_shnum = Nsect;
	hdr.e_shstrndx = 1;

	shdr[0].sh_name = 0;
	shdr[0].sh_type = SHT_NULL;
	shdr[0].sh_flags = 0;
	shdr[0].sh_addr = 0;
	shdr[0].sh_offset = 0;
	shdr[0].sh_size = 0;
	shdr[0].sh_link = SHN_UNDEF;
	shdr[0].sh_info = 0;
	shdr[0].sh_addralign = 0;
	shdr[0].sh_entsize = 0;

	shdr[1].sh_name = addstrtab(&shstrtab, ".shstrtab");
	shdr[1].sh_type = SHT_STRTAB;
	shdr[1].sh_flags = 0;
	shdr[1].sh_addr = 0;
	shdr[1].sh_link = 0;
	shdr[1].sh_info = 0;
	shdr[1].sh_addralign = 1;
	shdr[1].sh_entsize = 0;

	shdr[2].sh_name = addstrtab(&shstrtab, ".data");
	shdr[2].sh_type = SHT_PROGBITS;
	shdr[2].sh_flags = SHF_ALLOC|SHF_WRITE;
	shdr[2].sh_addr = 0;
	shdr[2].sh_link = 0;
	shdr[2].sh_info = 0;
	shdr[2].sh_addralign = align;
	shdr[2].sh_entsize = 0;

	shdr[3].sh_name = addstrtab(&shstrtab, ".strtab");
	shdr[3].sh_type = SHT_STRTAB;
	shdr[3].sh_flags = 0;
	shdr[3].sh_addr = 0;
	shdr[3].sh_link = 0;
	shdr[3].sh_info = 0;
	shdr[3].sh_addralign = 1;
	shdr[3].sh_entsize = 0;

	shdr[4].sh_name = addstrtab(&shstrtab, ".symtab");
	shdr[4].sh_type = SHT_SYMTAB;
	shdr[4].sh_flags = 0;
	shdr[4].sh_addr = 0;
	shdr[4].sh_link = 3;
	shdr[4].sh_info = 0; /* no locals */
	shdr[4].sh_addralign = 8;
	shdr[4].sh_entsize = sizeof(Elf64_Sym);

	addsymtab(&symtab,
		  addstrtab(&symstrtab, sym),
		  2,
		  0,
		  len);
	snprintf(endsym, sizeof(endsym), "end%s", sym);
	addsymtab(&symtab,
		  addstrtab(&symstrtab, endsym),
		  2,
		  len,
		  0);

	shstrtaboff = sizeof(Elf64_Ehdr)+Nsect*sizeof(Elf64_Shdr);
	dataoff = shstrtaboff+strtablen(&shstrtab);
	symstrtaboff = dataoff+len;
	symtaboff = symstrtaboff+strtablen(&symstrtab);
	
	shdr[1].sh_offset = shstrtaboff;
	shdr[1].sh_size = strtablen(&shstrtab);

	shdr[2].sh_offset = dataoff;
	shdr[2].sh_size = len;

	shdr[3].sh_offset = symstrtaboff;
	shdr[3].sh_size = strtablen(&symstrtab);

	shdr[4].sh_offset = symtaboff;
	shdr[4].sh_size = symtablen(&symtab);

	xwrite(fd, &hdr, sizeof(Elf64_Ehdr));
	xwrite(fd, shdr, Nsect*sizeof(Elf64_Shdr));
	xwrite(fd, shstrtab.tab, strtablen(&shstrtab));
	xwrite(fd, data, len);
	xwrite(fd, symstrtab.tab, strtablen(&symstrtab));
	xwrite(fd, symtab.tab, symtablen(&symtab));
}

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
	int ifd, ofd;
	int c;
	struct stat st;
	uptr align, len, maxlen, mlen;
	char *sym;
	void *data;
	char *ep;
	char *infile, *outfile;

	maxlen = 0;
	align = 0;
	len = 0;
	sym = 0;
	while(-1 != (c = getopt(argc, argv, "a:hl:s:")))
		switch(c){
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

	ifd = open(infile, O_RDONLY);
	if(0 > ifd){
		fprintf(stderr, "open: %s: %s\n", infile, strerror(errno));
		exit(1);
	}
	if(0 > fstat(ifd, &st)){
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
	if(data == MAP_FAILED){
		fprintf(stderr, "mmap: %s: %s\n", infile, strerror(errno));
		exit(1);
	}
	close(ifd);
	ofd = open(outfile, O_TRUNC|O_CREAT|O_WRONLY, 0664);
	if(0 > ofd){
		fprintf(stderr, "open: %s: %s\n", outfile, strerror(errno));
		exit(1);
	}
	mkelfo(ofd, sym, data, len, align);
	munmap(data, mlen);
	close(ofd);
	return 0;
}
