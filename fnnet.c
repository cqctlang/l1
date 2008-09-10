#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
freefdclose(Fd *fd)
{
	close(fd->fd);
}

static void
l1_opentcp(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *str;
	char *s;
	int xfd;
	struct sockaddr_in saddr;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to opentcp");
	checkarg(vm, "opentcp", argv, 0, Qstr);
	str = valstr(argv[0]);
	s = str2cstr(str);
	if(0 > parseip(s, &saddr))
		vmerr(vm, "unrecognized address: %.*s", (int)str->len, str->s);
	free(s);
	xfd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > xfd)
		vmerr(vm, "opentcp: %s", strerror(errno));
	if(0 > connect(xfd, (struct sockaddr*)&saddr, sizeof(saddr)))
		vmerr(vm, "opentcp: %s", strerror(errno));
	nodelay(xfd);
	fd = mkfd(str, xfd, Fread|Fwrite, freefdclose);
	*rv = mkvalfd(fd);
}

void
fnnet(Env *env)
{
	FN(opentcp);
}
