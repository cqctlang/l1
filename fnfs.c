#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
l1__readdir(VM *vm, Imm argc, Val *argv, Val *rv)
{
	DIR *dir;
	Str *names;
	char *name;
	u32 ndir, lim;
	char *buf;
	struct dirent *p, *d;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to readdir");
	checkarg(vm, "readdir", argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	dir = opendir(name);
	efree(name);
	if(dir == NULL)
		vmerr(vm, "opendir: %s", strerror(errno));

	lim = 128;
	buf = emalloc(lim*sizeof(struct dirent));
	ndir = 0;
	p = (struct dirent*)buf;
	while((d = readdir(dir))){
		if(ndir >= lim){
			buf = erealloc(buf, lim*sizeof(struct dirent),
				       2*lim*sizeof(struct dirent));
			lim *= 2;
			p = (struct dirent*)buf+ndir;
		}
		memcpy(p, d, d->d_reclen);
		p++;
		ndir++;
	}
	closedir(dir);
	*rv = mkvalas(mksas(mkstrk(buf, ndir*sizeof(struct dirent), Smalloc)));
}

static void
l1_mkdir(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *names;
	char *name;
	int r;
	
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkdir");
	checkarg(vm, "mkdir", argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	r = mkdir(name, 0777);	/* ~umask */
	efree(name);
	if(0 > r)
		vmerr(vm, "mkdir: %s", strerror(errno));
	/* return nil */
}
       
static void
l1_unlink(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *names;
	char *name;
	int r;
	
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to unlink");
	checkarg(vm, "unlink", argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	r = unlink(name);
	efree(name);
	if(0 > r)
		vmerr(vm, "unlink: %s", strerror(errno));
	/* return nil */
}

static void
l1_rmdir(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *names;
	char *name;
	int r;
	
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rmdir");
	checkarg(vm, "rmdir", argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	r = rmdir(name);
	efree(name);
	if(0 > r)
		vmerr(vm, "rmdir: %s", strerror(errno));
	/* return nil */
}

void
fnfs(Env *env)
{
	FN(_readdir);
	FN(mkdir);
	FN(rmdir);
	FN(unlink);
}
