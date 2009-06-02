#include "sys.h"
#include "util.h"
#include "syscqct.h"

enum {
	MaxMap = 1024,
};

typedef
struct Map
{
	u64 lo, hi;
} Map;

static void snap(VM *vm, Fd *fd);
static char* readmaps();
static Imm dowrite(VM *vm, Fd *fd, void *buf, Imm len);
static Map map[MaxMap];

static void
l1_snap(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to snap");
	checkarg(vm, "snap", argv, 0, Qfd);
	fd = valfd(argv[0]);
	snap(vm, fd);
}

static void
snap(VM *vm, Fd *fd)
{
	char *maps, *emaps, *p, r;
	u64 lo, hi, cnt;
	u32 i, nmap;
	Map *mp;

	if(fd->flags&Fclosed)
		vmerr(vm, "attempt to write to closed file descriptor");
	if((fd->flags&Fwrite) == 0)
		vmerr(vm, "attempt to write non-writable file descriptor");

	mp = map;
	nmap = 0;
	maps = readmaps();
	if(maps == 0)
		vmerr(vm, "cannot read memory map");
	emaps = maps+strlen(maps);
	p = maps;
	while(p < emaps){
		if(3 != sscanf(p, "%" PRIx64 "-%" PRIx64 " %c", &lo, &hi, &r)){
			efree(maps);
			vmerr(vm, "maps parse error");
		}
		if(r == 'r'){
			if(nmap >= MaxMap){
				efree(maps);
				vmerr(vm, "too many maps");
			}
			mp->lo = lo;
			mp->hi = hi;
			mp++;
			nmap++;
		}
		p = strchr(p, '\n');
		if(p == 0)
			break;
		p++;
	}
	efree(maps);

	/* hack: assume last map is vsyscall page, which despite
	   its perm mask cannot be read. */
	nmap--;

	if(-1 == dowrite(vm, fd, &nmap, sizeof(nmap)))
		vmerr(vm, "write: %s", strerror(errno));
	if(-1 == dowrite(vm, fd, map, nmap*sizeof(Map)))
		vmerr(vm, "write: %s", strerror(errno));
	mp = map;
	cnt = 0;
	for(i = 0; i < nmap; i++){
		if(-1 == dowrite(vm, fd, (void*)(uintptr_t)mp->lo,
				 mp->hi-mp->lo))
			vmerr(vm, "write: %s", strerror(errno));
		cnt += mp->hi-mp->lo;
		mp++;
	}
	fprintf(stderr, "%lu+%u*%lu+%" PRIu64 "=%" PRIu64 "\n",
		sizeof(nmap),
		nmap,
		sizeof(Map),
		cnt,
		sizeof(nmap)+nmap*sizeof(Map)+cnt);
	return;
}

static char*
readmaps()
{
	char name[128];
	int fd;
	unsigned m, max;
	char *buf;

	max = 4096;
	snprint(name, sizeof(name), "/proc/%d/maps", getpid());
again:
	buf = emalloc(max);
	fd = open(name, O_RDONLY);
	if(0 > fd){
		efree(buf);
		return 0;
	}
	m = xread(fd, buf, max-1);
	close(fd);
	if(m == -1){
		efree(buf);
		return 0;
	}
	if(m == max-1){
		/* maybe more to read */
		efree(buf);
		max *= 2;
		goto again;
	}
	buf[m+1] = 0;
	return buf;
}

static Imm
dowrite(VM *vm, Fd *fd, void *buf, Imm len)
{
	Val x;
	Val argv[3];
	Imm r;
	if(fd->flags&Ffn){
		if(!fd->u.fn.write)
			return 0;
		r = fd->u.fn.write(&fd->u.fn, buf, len);
		if(r == -1)
			return -1;
	}else{
		argv[0] = mkvalfd(fd);
		argv[1] = mkvalstr(mkstrk(buf, len, Sperm));
		argv[2] = mkvallitcval(Vuvlong, len);
		x = dovm(vm, fd->u.cl.write, 3, argv);
		if(x->qkind != Qnil)
			return -1;
	}
	return 0;
}

void
fnsnap(Env *env)
{
	FN(snap);
}
