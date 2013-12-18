#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "sys.h"
#include "util.h"
#include "syscqct.h"

#include "elf.h"

static void *
findheapimage64(u64 *len) {
	Elf64_Ehdr *e;
	Elf64_Shdr *sheader;
	Elf64_Shdr *strtab;
	int x;
	int exefd;
	unsigned char *header;
	struct stat statbuf;
	int rv;
	char *strs;

	exefd = open("/proc/self/exe", O_RDONLY);
	if(exefd == -1) {
		fprintf(stderr, "couldn't find heap in executable\n");
		return NULL;
	}

	rv = fstat(exefd, &statbuf);
	if(rv == -1) {
		fprintf(stderr, "couldn't stat executable\n");
		close(exefd);
		return NULL;
	}

	header=mmap(NULL, (statbuf.st_size + 4095) & ~0xfff , PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE, exefd, 0);
	if(header == (void *)-1) {
		fprintf(stderr, "couldn't map executable\n");
		close(exefd);
		return NULL;
	}

	close(exefd);

	e = (Elf64_Ehdr *)header;

	sheader=(Elf64_Shdr *)(header + e->e_shoff);

	if(!e->e_shstrndx) {
		fprintf(stderr, "couldn't find section with section names");
		munmap(header, (statbuf.st_size + 4095) & ~0xfff);
		return NULL;
	}
	strtab=(Elf64_Shdr *)(((unsigned char *)sheader) + e->e_shstrndx*e->e_shentsize);

	strs=(char *)(header + strtab->sh_offset);

	sheader=(Elf64_Shdr *)(header + e->e_shoff);
	for(x=0; x < e->e_shnum; x++) {
		if(!strcmp(strs + sheader->sh_name, "saveheap")) {
			*len = sheader->sh_size;
			return header + sheader->sh_offset;
		}
		sheader=(Elf64_Shdr *)(((unsigned char *)sheader) + e->e_shentsize);
	}

	munmap(header, (statbuf.st_size + 4095) & ~0xfff);
	return NULL;
}

void *findheapimage(u64 *len) {
	if(sizeof(long) == 8) {
		return findheapimage64(len);
	} else {
		return NULL;
	}
}
