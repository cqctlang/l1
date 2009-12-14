#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define WSTOPEVENT(STATUS) (((STATUS) & 0xff0000) >> 16)

static char *sig2str[] = {
	0,
	"SIGHUP",
	"SIGINT",
	"SIGQUIT",
	"SIGILL",
	"SIGTRAP",
	"SIGABRT",
	"SIGBUS",
	"SIGFPE",
	"SIGKILL",
	"SIGUSR1",
	"SIGSEGV",
	"SIGUSR2",
	"SIGPIPE",
	"SIGALRM",
	"SIGTERM",
	"SIGSTKFLT",
	"SIGCHLD",
	"SIGCONT",
	"SIGSTOP",
	"SIGTSTP",
	"SIGTTIN",
	"SIGTTOU",
	"SIGURG",
	"SIGXCPU",
	"SIGXFSZ",
	"SIGVTALRM",
	"SIGPROF",
	"SIGWINCH",
	"SIGIO",
	"SIGPWR",
	"SIGSYS",
};

static char *ev2str[] = {
	[PTRACE_EVENT_FORK] = "PTRACE_EVENT_FORK",
	[PTRACE_EVENT_VFORK] = "PTRACE_EVENT_VFORK",
	[PTRACE_EVENT_VFORK_DONE] = "PTRACE_EVENT_VFORK_DONE",
	[PTRACE_EVENT_EXEC] = "PTRACE_EVENT_EXEC",
	[PTRACE_EVENT_EXIT] = "PTRACE_EVENT_EXIT",
	[PTRACE_EVENT_CLONE] = "PTRACE_EVENT_CLONE",
};

static void
printstatus(int st)
{
	if(WIFEXITED(st))
		printf("WEXITSTATUS(%d)", WEXITSTATUS(st));
	else if(WIFSIGNALED(st) && WTERMSIG(st) < SIGRTMIN)
		printf("WTERMSIG(%s)", sig2str[WTERMSIG(st)]);
	else if(WIFSIGNALED(st))
		printf("WTERMSIG(%d)", WTERMSIG(st));
	else if(WIFSTOPPED(st) && WSTOPEVENT(st))
		/* assume stop sig is < SIGRTMIN */
		printf("WSTOPSIG(%s) for %s",
		       sig2str[WSTOPSIG(st)],
		       ev2str[WSTOPEVENT(st)]);
	else if(WIFSTOPPED(st) && WSTOPSIG(st) < SIGRTMIN)
		printf("WSTOPSIG(%s)", sig2str[WSTOPSIG(st)]);
	else if(WIFSTOPPED(st))
		printf("WSTOPSIG(%d)", WSTOPSIG(st));
	else if(WIFCONTINUED(st))
		printf("CONTINUED");
	else
		printf("unrecognized status %x", st);
}

int
main(int argc, char *argv[])
{
	int pid, st;

	pid = fork();
	if(pid == 0){
//		printf("about to SIGSTOP myself (%d)\n", getpid());
//		kill(getpid(), SIGTSTP);
//		printf("resumed from SIGSTOP (%d)\n", getpid());
		printf("about to exit (%d)\n", getpid());
		exit(0);
	}else if(pid == -1){
		printf("fork: %s\n", strerror(errno));
		exit(1);
	}else{
		st = -1;
		while(!WIFEXITED(st)){
			waitpid(pid, &st, WUNTRACED);
			printf("waited on %d: ", pid);
			printstatus(st);
			printf("\n");
		}
	}

	return 0;
}
