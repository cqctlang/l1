{
	Elf_Ehdr hdr;
	Elf_Shdr shdr[Nsect];
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
	hdr.e_ident[EI_CLASS] = sys->width;
	hdr.e_ident[EI_DATA] = ELFDATA2LSB;
	hdr.e_ident[EI_VERSION] = EV_CURRENT;
	hdr.e_ident[EI_OSABI] = ELFOSABI_SYSV;
	hdr.e_type = ET_REL;
	hdr.e_machine = sys->machine;
	hdr.e_version = EV_CURRENT;
	hdr.e_entry = 0;
	hdr.e_phoff = 0;
	hdr.e_shoff = sizeof(Elf_Ehdr);
	hdr.e_flags = 0;
	hdr.e_ehsize = sizeof(Elf_Ehdr);
	hdr.e_phentsize = 0;
	hdr.e_phnum = 0;
	hdr.e_shentsize = sizeof(Elf_Shdr);
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
	shdr[4].sh_entsize = sizeof(Elf_Sym);

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

	shstrtaboff = sizeof(Elf_Ehdr)+Nsect*sizeof(Elf_Shdr);
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

	xwrite(fd, &hdr, sizeof(Elf_Ehdr));
	xwrite(fd, shdr, Nsect*sizeof(Elf_Shdr));
	xwrite(fd, shstrtab.tab, strtablen(&shstrtab));
	xwrite(fd, data, len);
	xwrite(fd, symstrtab.tab, strtablen(&symstrtab));
	xwrite(fd, symtab.tab, symtablen(&symtab));
}
