
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/un.h>
#include <sys/ptrace.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#ifdef __linux__
#include <linux/fs.h>
#include <sched.h>
#endif
#ifdef __APPLE__
#include <sys/disk.h>
#endif

struct enum_entry {
	const char *name;
	long value;
};

void
print_enum(const char *name,struct enum_entry entries[]) {
	struct enum_entry *cursor;

	printf("enum %s {\n", name);
	cursor=entries;
	for(cursor=entries;*(void **)cursor && cursor->name;cursor++) {
		printf("\t%s\t= 0x%lx,\n",cursor->name,cursor->value);
	}
	printf("};\n");
}

int
main(int argc,char **argv) {

	struct enum_entry oflags[]={
		{ "O_RDONLY",	O_RDONLY },
		{ "O_WRONLY",	O_WRONLY },
		{ "O_RDWR", 	O_RDWR },	
		{ "O_CREAT", 	O_CREAT },	
		{ "O_NOCTTY",	O_NOCTTY },
		{ "O_TRUNC", 	O_TRUNC },	
		{ "O_APPEND", 	O_APPEND },	
		{ "O_NONBLOCK", O_NONBLOCK },	
#ifdef O_ASYNC
		{ "O_ASYNC", 	O_ASYNC },	
#endif
#ifdef O_SYNC
		{ "O_SYNC", 	O_SYNC },	
#endif
#ifdef O_EXCL
		{ "O_EXCL",	O_EXCL },
#endif
#ifdef O_CLOEXEC // linux since 2.6.23
		{ "O_CLOEXEC", 	O_CLOEXEC },	
#endif
#ifdef O_DIRECT // linux since 2.4.10
		{ "O_DIRECT", 	O_DIRECT },	
#endif
		{ NULL },
	};

	struct enum_entry aflags[]={
		{ "R_OK",	R_OK },
		{ "W_OK",	W_OK },
		{ "X_OK",	X_OK },
		{ "F_OK",	F_OK },
		{ NULL },
	};

	struct enum_entry seekflags[]={
		{ "SEEK_SET",	SEEK_SET },
		{ "SEEK_CUR",	SEEK_CUR },	
		{ "SEEK_END",	SEEK_END },
		{ NULL },
	};

#ifdef SYS_clone
	struct enum_entry cloneflags[]={
#ifdef	CLONE_CHILD_CLEARTID // 2.5.49
		{ "CLONE_CHILD_CLEARTID",	CLONE_CHILD_CLEARTID },
#endif
#ifdef	CLONE_CHILD_SETTID // 2.5.49
		{ "CLONE_CHILD_SETTID",		CLONE_CHILD_SETTID },
#endif
#ifdef	CLONE_FILES
		{ "CLONE_FILES",		CLONE_FILES },
#endif
#ifdef	CLONE_FS
		{ "CLONE_FILES",		CLONE_FILES },
#endif
#ifdef	CLONE_IO // 2.6.25
		{ "CLONE_IO",			CLONE_IO },
#endif
#ifdef	CLONE_NEWIPC // 2.6.19
		{ "CLONE_NEWIPC",		CLONE_NEWIPC },
#endif
#ifdef	CLONE_NEWNET // 2.6.24
		{ "CLONE_NEWNET",		CLONE_NEWNET },
#endif
#ifdef	CLONE_NEWNS // 2.6.19
		{ "CLONE_NEWNS",		CLONE_NEWNS },
#endif
#ifdef	CLONE_NEWPID // 2.6.24
		{ "CLONE_NEWPID",		CLONE_NEWPID },
#endif
#ifdef	CLONE_NEWUTS // 2.6.19
		{ "CLONE_NEWUTS",		CLONE_NEWUTS },
#endif
#ifdef	CLONE_PARENT // 2.3.12
		{ "CLONE_PARENT",		CLONE_PARENT },
#endif
#ifdef	CLONE_PARENT_SETTID // 2.5.49
		{ "CLONE_PARENT_SETTID",	CLONE_PARENT_SETTID },
#endif
#ifdef	CLONE_PID // long obsolete
		{ "CLONE_PID",			CLONE_PID },
#endif
#ifdef	CLONE_PTRACE
		{ "CLONE_PTRACE",		CLONE_PTRACE },
#endif
#ifdef	CLONE_SETTLS // 2.5.32
		{ "CLONE_SETTLS",		CLONE_SETTLS },
#endif
#ifdef	CLONE_SIGHAND
		{ "CLONE_SIGHAND",		CLONE_SIGHAND },
#endif
#ifdef	CLONE_STOPPED // 2.6.0--test2
		{ "CLONE_STOPPED",		CLONE_STOPPED },
#endif
#ifdef	CLONE_SYSVSEM // 2.5.10
		{ "CLONE_SYSVSEM",		CLONE_SYSVSEM },
#endif
#ifdef	CLONE_THREAD // 2.4.0-test8
		{ "CLONE_THREAD",		CLONE_THREAD },
#endif
#ifdef	CLONE_UNTRACED // 2.5.46
		{ "CLONE_UNTRACED",		CLONE_UNTRACED },
#endif
#ifdef	CLONE_VFORK
		{ "CLONE_VFORK",		CLONE_VFORK },
#endif
#ifdef	CLONE_VM
		{ "CLONE_VM",			CLONE_VM },
#endif
		{ NULL },
	};
#endif // Darwin misses this

	struct enum_entry waitflags[]={
		{ "WNOHANG",	WNOHANG },
		{ "WUNTRACED",	WUNTRACED },
#ifdef	WEXITED // Linux > 2.6.9
		{ "WEXITED",	WEXITED },
#endif
#ifdef	WSTOPPED // Linux > 2.6.9
		{ "WSTOPPED",	WSTOPPED },
#endif
#ifdef	WCONTINUED // Linux > 2.6.10
		{ "WCONTINUED",	WCONTINUED },
#endif
#ifdef	WNOWAIT // Linux > 2.6.9
		{ "WNOWAIT",	WNOWAIT },
#endif
		{ NULL },
	};

	struct enum_entry mmap_consts[]={
		{ "PROT_READ",		PROT_READ },
		{ "PROT_WRITE",		PROT_WRITE },
		{ "PROT_EXEC",		PROT_EXEC },
		{ "MAP_SHARED",		MAP_SHARED },
		{ "MAP_PRIVATE",	MAP_PRIVATE },
#ifdef MAP_LOCKED // Darwin misses this
		{ "MAP_LOCKED",		MAP_LOCKED },
#endif
#ifdef MAP_NORESERVE // Darwin misses this
		{ "MAP_NORESERVE",	MAP_NORESERVE },
#endif
		{ NULL },
	};

	struct enum_entry address_families[]={
		{ "AF_UNSPEC",		AF_UNSPEC },
		{ "AF_UNIX",		AF_UNIX },
		{ "AF_LOCAL",		AF_LOCAL },
		{ "AF_INET",		AF_INET },
		{ "AF_INET6",		AF_INET6 },
		{ NULL },

	};

	struct enum_entry socket_type[]={
		{ "SOCK_STREAM",	SOCK_STREAM },
		{ "SOCK_DGRAM",		SOCK_DGRAM },
		{ "SOCK_RAW",		SOCK_RAW },
		{ NULL },
	};

	struct enum_entry sockopts[]={
		{ "SOL_SOCKET",		SOL_SOCKET }, // available on Darwin


		{ "SO_DEBUG",		SO_DEBUG }, // available on Darwin
		{ "SO_REUSEADDR",	SO_REUSEADDR }, // available on Darwin
#ifdef SO_REUSEPORT
		{ "SO_REUSEPORT",	SO_REUSEPORT }, // available on Darwin
#endif
		{ "SO_KEEPALIVE",	SO_KEEPALIVE }, // available on Darwin
		{ "SO_DONTROUTE",	SO_DONTROUTE }, // available on Darwin
		{ "SO_LINGER",		SO_LINGER }, // available on Darwin
		{ "SO_OOBINLINE",	SO_OOBINLINE }, // available on Darwin
		{ "SO_SNDBUF",		SO_SNDBUF }, // available on Darwin
		{ "SO_RCVBUF",		SO_RCVBUF }, // available on Darwin
		{ "SO_SNDLOWAT",	SO_SNDLOWAT }, // available on Darwin
		{ "SO_RCVLOWAT",	SO_RCVLOWAT }, // available on Darwin
		{ "SO_SNDTIMEO",	SO_SNDTIMEO }, // available on Darwin
		{ "SO_RCVTIMEO",	SO_RCVTIMEO }, // available on Darwin
		{ "SO_TYPE",		SO_TYPE }, // available on Darwin
		{ "SO_ERROR",		SO_ERROR }, // available on Darwin
#ifdef SO_NOSIGPIPE
		{ "SO_NOSIGPIPE",	SO_NOSIGPIPE }, // available on Darwin
#endif
#ifdef SO_NREAD
		{ "SO_NREAD",		SO_NREAD }, // available on Darwin
#endif
#ifdef SO_NWRITE
		{ "SO_NWRITE",		SO_NWRITE }, // available on Darwin
#endif
#ifdef SO_LINGER_SEC
		{ "SO_LINGER_SEC",	SO_LINGER_SEC }, // available on Darwin
#endif
#ifdef SO_BROADCAST
		{ "SO_BROADCAST",	SO_BROADCAST },
#endif
#ifdef SO_BINDTODEVICE
		{ "SO_BINDTODEVICE",	SO_BINDTODEVICE },
#endif
		{ NULL },
	};

	struct enum_entry tcp[]={
		{ "INADDR_ANY",	INADDR_ANY},
		{ NULL },
	};

	struct enum_entry tcp_sockopt[]={
		{ "IPPROTO_TCP",		IPPROTO_TCP },

		{ "TCP_NODELAY",		TCP_NODELAY },
		{ "TCP_MAXSEG",			TCP_MAXSEG },
#ifdef	TCP_NOOPT
		{ "TCP_NOOPT",			TCP_NOOPT }, // available on Darwin
#endif
#ifdef TCP_NOPUSH
		{ "TCP_NOPUSH",			TCP_NOPUSH }, // available on Darwin
#endif
#ifdef TCP_KEEPALIVE
		{ "TCP_KEEPALIVE",		TCP_KEEPALIVE }, // available on Darwin
#endif
#ifdef TCP_CONNECTIONTIMEOUT
		{ "TCP_CONNECTIONTIMEOUT",	TCP_CONNECTIONTIMEOUT }, // available on Darwin
#endif
		{ NULL },
	};

#ifdef UNIX_PATH_MAX
	struct enum_entry un_detail[]={
		{ "UNIX_PATH_MAX",		UNIX_PATH_MAX },
		{ NULL },
	};
#endif

	struct enum_entry ioctl_nr[]={
#ifdef BLKGETSIZE // Darwin misses this
		{ "BLKGETSIZE",		BLKGETSIZE },
#endif
#ifdef __APPLE__
		{ "DKIOCGETBLOCKCOUNT", DKIOCGETBLOCKCOUNT },
#endif
#ifdef TCGETS
		{ "TCGETS",		TCGETS },
#endif
#ifdef TCSETSW
		{ "TCSETSW",		TCSETSW },
#endif
		{ NULL },
	};

	struct enum_entry termios_nr[]={
		/* c_cc characters */
		{ "VINTR",	VINTR},
		{ "VQUIT",	VQUIT},
		{ "VERASE",	VERASE},
		{ "VKILL",	VKILL},
		{ "VEOF",	VEOF},
		{ "VTIME",	VTIME},
		{ "VMIN",	VMIN},
#ifdef VSWTC
		{ "VSWTC",	VSWTC},
#endif
		{ "VSTART",	VSTART},
		{ "VSTOP",	VSTOP},
		{ "VSUSP",	VSUSP},
		{ "VEOL",	VEOL},
		{ "VREPRINT",	VREPRINT},
		{ "VDISCARD",	VDISCARD},
		{ "VWERASE",	VWERASE},
		{ "VLNEXT",	VLNEXT},
		{ "VEOL2",	VEOL2},
		/* c_iflag bits */
		{ "IGNBRK",	IGNBRK},
		{ "BRKINT",	BRKINT},
		{ "IGNPAR",	IGNPAR},
		{ "PARMRK",	PARMRK},
		{ "INPCK",	INPCK},
		{ "ISTRIP",	ISTRIP},
		{ "INLCR",	INLCR},
		{ "IGNCR",	IGNCR},
		{ "ICRNL",	ICRNL},
#ifdef IUCLC
		{ "IUCLC",	IUCLC},
#endif
		{ "IXON",	IXON},
		{ "IXANY",	IXANY},
		{ "IXOFF",	IXOFF},
		{ "IMAXBEL",	IMAXBEL},
		{ "IUTF8",	IUTF8},
		/* c_oflag bits */
		{ "OPOST",	OPOST},
#ifdef OLCUC
		{ "OLCUC",	OLCUC},
#endif
		{ "ONLCR",	ONLCR},
		{ "OCRNL",	OCRNL},
		{ "ONOCR",	ONOCR},
		{ "ONLRET",	ONLRET},
		{ "OFILL",	OFILL},
		{ "OFDEL",	OFDEL},
		/* c_cflag bit meaning */
#ifdef CBAUD
		{ "CBAUD",	CBAUD},
#endif
		{ "B0",		B0},
		{ "B50",	B50},
		{ "B75",	B75},
		{ "B110",	B110},
		{ "B134",	B134},
		{ "B150",	B150},
		{ "B200",	B200},
		{ "B300",	B300},
		{ "B600",	B600},
		{ "B1200",	B1200},
		{ "B1800",	B1800},
		{ "B2400",	B2400},
		{ "B4800",	B4800},
		{ "B9600",	B9600},
		{ "B19200",	B19200},
		{ "B38400",	B38400},
		{ "CSIZE",	CSIZE},
		{ "CS5",	CS5},
		{ "CS6",	CS6},
		{ "CS7",	CS7},
		{ "CS8",	CS8},
		{ "CSTOPB",	CSTOPB},
		{ "CREAD",	CREAD},
		{ "PARENB",	PARENB},
		{ "PARODD",	PARODD},
		{ "HUPCL",	HUPCL},
		{ "CLOCAL",	CLOCAL},
		{ "B57600",	B57600},
		{ "B115200",	B115200},
		{ "B230400",	B230400},
#ifdef B460800
		{ "B460800",	B460800},
		{ "B500000",	B500000},
		{ "B576000",	B576000},
		{ "B921600",	B921600},
		{ "B1000000",	B1000000},
		{ "B1152000",	B1152000},
		{ "B1500000",	B1500000},
		{ "B2000000",	B2000000},
		{ "B2500000",	B2500000},
		{ "B3000000",	B3000000},
		{ "B3500000",	B3500000},
		{ "B4000000",	B4000000},
#endif
#ifdef __MAX_BAUD
		{ "__MAX_BAUD",	__MAX_BAUD},
#endif
		/* c_lflag bits */
		{ "ISIG",	ISIG},
		{ "ICANON",	ICANON},
		{ "ECHO",	ECHO},
		{ "ECHOE",	ECHOE},
		{ "ECHOK",	ECHOK},
		{ "ECHONL",	ECHONL},
		{ "NOFLSH",	NOFLSH},
		{ "TOSTOP",	TOSTOP},
		{ "IEXTEN",	IEXTEN},

		{ "TCOOFF",	TCOOFF},
		{ "TCOON",	TCOON},
		{ "TCIOFF",	TCIOFF},
		{ "TCION",	TCION},

		{ "TCIFLUSH",	TCIFLUSH},
		{ "TCOFLUSH",	TCOFLUSH},
		{ "TCIOFLUSH",	TCIOFLUSH},

		{ "TCSANOW",	TCSANOW},
		{ "TCSADRAIN",	TCSADRAIN},
		{ "TCSAFLUSH",	TCSAFLUSH},

		{ NULL },
	};

	struct enum_entry signal_nr[]={
		{ "SIGHUP",	SIGHUP},
		{ "SIGINT",	SIGINT},
		{ "SIGQUIT",	SIGQUIT},
		{ "SIGILL",	SIGILL},
		{ "SIGTRAP",	SIGTRAP},
		{ "SIGABRT",	SIGABRT},
#ifdef SIGEMT
		{ "SIGEMT",	SIGEMT},
#endif
		{ "SIGFPE",	SIGFPE},
		{ "SIGKILL",	SIGKILL},
#ifdef SIGBUS
		{ "SIGBUS",	SIGBUS},
#endif
		{ "SIGSEGV",	SIGSEGV},
		{ "SIGSYS",	SIGSYS},
		{ "SIGPIPE",	SIGPIPE},
		{ "SIGALRM",	SIGALRM},
		{ "SIGTERM",	SIGTERM},
		{ "SIGURG",	SIGURG},
		{ "SIGSTOP",	SIGSTOP},
		{ "SIGTSTP",	SIGTSTP},
		{ "SIGCONT",	SIGCONT},
		{ "SIGCHLD",	SIGCHLD},
		{ "SIGTTIN",	SIGTTIN},
		{ "SIGTTOU",	SIGTTOU},
		{ "SIGIO",	SIGIO},
		{ "SIGXCPU",	SIGXCPU},
		{ "SIGXFSZ",	SIGXFSZ},
		{ "SIGVTALRM",	SIGVTALRM},
		{ "SIGPROF",	SIGPROF},
		{ "SIGWINCH",	SIGWINCH},
#ifdef SIGINFO
		{ "SIGINFO",	SIGINFO},
#endif
		{ "SIGUSR1",	SIGUSR1},
		{ "SIGUSR2",	SIGUSR2},

		// not a signal number
		{ "SIG_IGN",	(long)SIG_IGN},
		{ NULL },
	};

	struct enum_entry mount_nr[]={
#ifdef MS_NOATIME
		{ "MS_BIND",		MS_BIND},
		{ "MS_DIRSYNC",		MS_DIRSYNC},
		{ "MS_MANDLOCK",	MS_MANDLOCK},
		{ "MS_MOVE",		MS_MOVE},
		{ "MS_NOATIME",		MS_NOATIME},
		{ "MS_NODEV",		MS_NODEV},
		{ "MS_NODIRATIME",	MS_NODIRATIME},
		{ "MS_NOEXEC",		MS_NOEXEC},
		{ "MS_NOEXEC",		MS_NOEXEC},
		{ "MS_NOSUID",		MS_NOSUID},
		{ "MS_RDONLY",		MS_RDONLY},
		{ "MS_RELATIME",	MS_RELATIME},
		{ "MS_REMOUNT",		MS_REMOUNT},
		{ "MS_SILENT",		MS_SILENT},
		{ "MS_STRICTATIME",	MS_STRICTATIME},
		{ "MS_SYNCHRONOUS",	MS_SYNCHRONOUS},
#endif
		{ NULL },
	};

	struct enum_entry ptrace[]={
		// generic names
		{ "PT_TRACE_ME", PT_TRACE_ME },

#ifdef PTRACE_TRACEME
		// linux names
		{ "PTRACE_TRACEME", PTRACE_TRACEME },
		{ "PTRACE_PEEKTEXT", PTRACE_PEEKTEXT },
		{ "PTRACE_PEEKDATA", PTRACE_PEEKDATA },
		{ "PTRACE_PEEKUSER", PTRACE_PEEKUSER },
		{ "PTRACE_POKETEXT", PTRACE_POKETEXT },
		{ "PTRACE_POKEDATA", PTRACE_POKEDATA },
		{ "PTRACE_POKEUSER", PTRACE_POKEUSER },
		{ "PTRACE_GETREGS", PTRACE_GETREGS },
		{ "PTRACE_GETFPREGS", PTRACE_GETFPREGS },
		{ "PTRACE_GETSIGINFO", PTRACE_GETSIGINFO },
		{ "PTRACE_SETREGS", PTRACE_SETREGS },
		{ "PTRACE_SETSIGINFO", PTRACE_SETSIGINFO },
		{ "PTRACE_SETOPTIONS", PTRACE_SETOPTIONS },
			{ "PTRACE_O_TRACESYSGOOD", PTRACE_O_TRACESYSGOOD },
			{ "PTRACE_O_TRACEFORK", PTRACE_O_TRACEFORK },
			{ "PTRACE_O_TRACEVFORK", PTRACE_O_TRACEVFORK },
			{ "PTRACE_O_TRACECLONE", PTRACE_O_TRACECLONE },
			{ "PTRACE_O_TRACEEXEC", PTRACE_O_TRACEEXEC },
			{ "PTRACE_O_TRACEVFORKDONE", PTRACE_O_TRACEVFORKDONE },
			{ "PTRACE_O_TRACEEXIT", PTRACE_O_TRACEEXIT },
		{ "PTRACE_GETEVENTMSG", PTRACE_GETEVENTMSG },
		{ "PTRACE_CONT", PTRACE_CONT },
		{ "PTRACE_SYSCALL", PTRACE_SYSCALL },
		{ "PTRACE_SINGLESTEP", PTRACE_SINGLESTEP },
#ifdef PTRACE_SYSEMU
		{ "PTRACE_SYSEMU", PTRACE_SYSEMU },
		{ "PTRACE_SYSEMU_SINGLESTEP", PTRACE_SYSEMU_SINGLESTEP },
#endif
		{ "PTRACE_KILL", PTRACE_KILL },
		{ "PTRACE_ATTACH", PTRACE_ATTACH },
		{ "PTRACE_DETACH", PTRACE_DETACH },
#endif
#ifdef PT_TRACE_ME // true on Darwin
		{ "PT_TRACE_ME", PT_TRACE_ME },
		{ "PT_DENY_ATTACH", PT_DENY_ATTACH },
		{ "PT_CONTINUE", PT_CONTINUE },
		{ "PT_STEP", PT_STEP },
		{ "PT_KILL", PT_KILL },
		{ "PT_ATTACH", PT_ATTACH },
		{ "PT_DETACH", PT_DETACH },
#endif
		{ NULL },
	};

	struct enum_entry syscall_nr[]={
		{ "read",	SYS_read },
		{ "write",	SYS_write },
		{ "open",	SYS_open },
		{ "close",	SYS_close },
		{ "stat",	SYS_stat },
#ifdef SYS_stat64
		{ "stat64",	SYS_stat64 },
#endif
		{ "fstat",	SYS_fstat },
#ifdef SYS_fstat64
		{ "fstat64",	SYS_fstat64 },
#endif
		{ "lstat",	SYS_lstat },
#ifdef SYS_lstat64
		{ "lstat64",	SYS_lstat64 },
#endif
		{ "poll",	SYS_poll },
		{ "lseek",	SYS_lseek },
#ifdef SYS_mmap // replaced by mmap2 in some places
		{ "mmap",	SYS_mmap },
#endif
#ifdef SYS_mmap2 // many linux
		{ "mmap2",	SYS_mmap2 },
#endif
		{ "mprotect",	SYS_mprotect },
		{ "munmap",	SYS_munmap },
#ifdef SYS_brk // Darwin misses this
		{ "brk",	SYS_brk },
#endif
#ifdef SYS_rt_sigaction // Darwin misses this
		{ "rt_sigaction",	SYS_rt_sigaction },
		{ "rt_sigprocmask",	SYS_rt_sigprocmask },
		{ "rt_sigreturn",	SYS_rt_sigreturn },
#endif
		{ "ioctl",		SYS_ioctl },
#ifdef SYS_pread64 // Darwin misses this
		{ "pread64",		SYS_pread64 },
		{ "pwrite64",		SYS_pwrite64 },
#endif
		{ "readv",		SYS_readv },
		{ "writev",		SYS_writev },
		{ "access",		SYS_access },
		{ "pipe",		SYS_pipe },
#ifdef SYS_select // replaced by newselect some places
		{ "select",		SYS_select },
#endif
#ifdef SYS__newselect // some linux
		{ "_newselect",		SYS__newselect },
#endif
#ifdef SYS_sched_yield // Darwin misses this
		{ "sched_yield",	SYS_sched_yield },
#endif
#ifdef SYS_mremap // Darwin misses this
		{ "mremap",		SYS_mremap },
#endif
		{ "msync",		SYS_msync },
		{ "mincore",		SYS_mincore },
		{ "madvise",		SYS_madvise },
#ifdef SYS_shmget // linux i386 misses these
		{ "shmget",		SYS_shmget },
		{ "shmat",		SYS_shmat },
		{ "shmctl",		SYS_shmctl },
#endif
		{ "dup",		SYS_dup },
		{ "dup2",		SYS_dup2 },
#ifdef SYS_pause // Darwin misses this
		{ "pause",		SYS_pause },
#endif
#ifdef SYS_nanosleep // Darwin misses this
		{ "nanosleep",		SYS_nanosleep },
#endif
		{ "getitimer",		SYS_getitimer },
#ifdef SYS_alarm // Darwin misses this
		{ "alarm",		SYS_alarm },
#endif
		{ "setitimer",		SYS_setitimer },
		{ "getpid",		SYS_getpid },
		{ "sendfile",		SYS_sendfile },
#ifdef SYS_socket // linux i386 uses socketcall
		{ "socket",		SYS_socket },
		{ "connect",		SYS_connect },
		{ "accept",		SYS_accept },
		{ "sendto",		SYS_sendto },
		{ "recvfrom",		SYS_recvfrom },
		{ "sendmsg",		SYS_sendmsg },
		{ "recvmsg",		SYS_recvmsg },
		{ "shutdown",		SYS_shutdown },
		{ "bind",		SYS_bind },
		{ "listen",		SYS_listen },
		{ "getsockname",	SYS_getsockname },
		{ "getpeername",	SYS_getpeername },
		{ "socketpair",		SYS_socketpair },
		{ "setsockopt",		SYS_setsockopt },
		{ "getsockopt",		SYS_getsockopt },
#endif
#ifdef SYS_clone // Darwin misses this
		{ "clone",		SYS_clone },
#endif
		{ "fork",		SYS_fork },
		{ "vfork",		SYS_vfork },
		{ "execve",		SYS_execve },
		{ "exit",		SYS_exit },
		{ "wait4",		SYS_wait4 },
		{ "kill",		SYS_kill },
#ifdef SYS_uname // Darwin misses this
		{ "uname",		SYS_uname },
#endif
#ifdef SYS_shmget // linux i386 misses these
		{ "semget",		SYS_semget },
		{ "semop",		SYS_semop },
		{ "semctl",		SYS_semctl },
		{ "shmdt",		SYS_shmdt },
		{ "msgget",		SYS_msgget },
		{ "msgsnd",		SYS_msgsnd },
		{ "msgrcv",		SYS_msgrcv },
		{ "msgctl",		SYS_msgctl },
#endif
		{ "fcntl",		SYS_fcntl },
		{ "flock",		SYS_flock },
		{ "fsync",		SYS_fsync },
		{ "fdatasync",		SYS_fdatasync },
		{ "truncate",		SYS_truncate },
		{ "ftruncate",		SYS_ftruncate },
#ifdef SYS_getdents // Darwin misses this
		{ "getdents",		SYS_getdents },
#endif
#ifdef SYS_getcwd // Darwin misses this
		{ "getcwd",		SYS_getcwd },
#endif
		{ "chdir",		SYS_chdir },
		{ "fchdir",		SYS_fchdir },
		{ "rename",		SYS_rename },
		{ "mkdir",		SYS_mkdir },
		{ "rmdir",		SYS_rmdir },
#ifdef SYS_creat // Darwin misses this
		{ "creat",		SYS_creat },
#endif
		{ "link",		SYS_link },
		{ "unlink",		SYS_unlink },
		{ "symlink",		SYS_symlink },
		{ "readlink",		SYS_readlink },
		{ "chmod",		SYS_chmod },
		{ "fchmod",		SYS_fchmod },
		{ "chown",		SYS_chown },
		{ "fchown",		SYS_fchown },
		{ "lchown",		SYS_lchown },
		{ "umask",		SYS_umask },
		{ "gettimeofday",	SYS_gettimeofday },
#ifdef SYS_getrlimit // some linux miss this 
		{ "getrlimit",		SYS_getrlimit },
#endif
		{ "getrusage",		SYS_getrusage },
#ifdef SYS_sysinfo // Darwin misses this
		{ "sysinfo",		SYS_sysinfo },
#endif
#ifdef SYS_times // Darwin misses this
		{ "times",		SYS_times },
#endif
		{ "ptrace",		SYS_ptrace },
		{ "getuid",		SYS_getuid },
#ifdef SYS_syslog // Darwin misses this
		{ "syslog",		SYS_syslog },
#endif
		{ "getgid",		SYS_getgid },
		{ "setuid",		SYS_setuid },
		{ "setgid",		SYS_setgid },
		{ "geteuid",		SYS_geteuid },
		{ "getegid",		SYS_getegid },
		{ "setpgid",		SYS_setpgid },
		{ "getppid",		SYS_getppid },
		{ "getpgrp",		SYS_getpgrp },
		{ "setsid",		SYS_setsid },
		{ "setreuid",		SYS_setreuid },
		{ "setregid",		SYS_setregid },
		{ "getgroups",		SYS_getgroups },
		{ "setgroups",		SYS_setgroups },
#ifdef SYS_setresuid // Darwin misses this
		{ "setresuid",		SYS_setresuid },
		{ "getresuid",		SYS_getresuid },
		{ "setresgid",		SYS_setresgid },
		{ "getresgid",		SYS_getresgid },
#endif
		{ "getpgid",		SYS_getpgid },
#ifdef SYS_setfsuid // Darwin misses this
		{ "setfsuid",		SYS_setfsuid },
		{ "setfsgid",		SYS_setfsgid },
#endif
		{ "getsid",		SYS_getsid },
#ifdef SYS_capget // Darwin misses this
		{ "capget",		SYS_capget },
		{ "capset",		SYS_capset },
#endif
#ifdef SYS_rt_sigpending // Darwin misses this
		{ "rt_sigpending",	SYS_rt_sigpending },
		{ "rt_sigtimedwait",	SYS_rt_sigtimedwait },
		//{ "rt_sigqueueinfo",	SYS_sigqueueinfo },
		//{ "rt_sigsuspend",	SYS_sigsuspend },
#endif
		//{ "signaltstack",	SYS_signalstack },
#ifdef SYS_utime // Darwin misses this
		{ "utime",		SYS_utime },
#endif
		{ "mknod",		SYS_mknod },
#ifdef SYS_uselib // Darwin misses this
		{ "uselib",		SYS_uselib },
#endif
#ifdef SYS_personality // Darwin misses this
		{ "personality",	SYS_personality },
#endif
#ifdef SYS_ustat // Darwin misses this
		{ "ustat",		SYS_ustat },
#endif
		{ "statfs",		SYS_statfs },
		{ "fstatfs",		SYS_fstatfs },
#ifdef SYS_sysfs // Darwin misses this
		{ "sysfs",		SYS_sysfs },
#endif
		{ "getpriority",	SYS_getpriority },
		{ "setpriority",		SYS_setpriority },
#ifdef SYS_sched_setparam
		{ "sched_setparam",		SYS_sched_setparam },
		{ "sched_getparam",		SYS_sched_getparam },
		{ "sched_setscheduler",		SYS_sched_setscheduler },
		{ "sched_getscheduler",		SYS_sched_getscheduler },
		{ "sched_get_priority_max",	SYS_sched_get_priority_max },
		{ "sched_get_priority_min",	SYS_sched_get_priority_min },
		{ "sched_rr_get_interval",	SYS_sched_rr_get_interval },
#endif
		{ "mlock",		SYS_mlock },
		{ "munlock",		SYS_munlock },
		{ "mlockall",		SYS_mlockall },
		{ "munlockall",		SYS_munlockall },
#ifdef SYS_vhangup
		{ "vhangup",		SYS_vhangup },
#endif
#ifdef SYS_modify_ldt
		{ "modify_ldt",		SYS_modify_ldt },
#endif
#ifdef SYS_pivot_root
		{ "pivot_root",		SYS_pivot_root },
#endif
		//{ "_sysctl",		SYS_
#ifdef SYS_prctl
		{ "prctl",		SYS_prctl },
#endif
#ifdef SYS_arch_prctl // linux i386 misses this
		{ "arch_prctl",		SYS_arch_prctl },
#endif
#ifdef SYS_adjtimex // Darwin misses this
		{ "adjtimex",		SYS_adjtimex },
#endif
		{ "setrlimit",		SYS_setrlimit },
		{ "chroot",		SYS_chroot },
		{ "sync",		SYS_sync },
		{ "acct",		SYS_acct },
		{ "settimeofday",	SYS_settimeofday },
		{ "mount",		SYS_mount },
#ifdef SYS_umount2
		{ "umount2",		SYS_umount2 },
#endif
#ifdef SYS_swapon
		{ "swapon",		SYS_swapon },
#endif
#ifdef SYS_swapoff
		{ "swapoff",		SYS_swapoff },
#endif
#ifdef SYS_reboot
		{ "reboot",		SYS_reboot },
#endif
#ifdef		SYS_sethostname
		{ "sethostname",	SYS_sethostname },
		{ "setdomainname",	SYS_setdomainname },
#endif
#ifdef 		SYS_iopl
		{ "iopl",		SYS_iopl },
#endif
#ifdef SYS_ioperm
		{ "ioperm",		SYS_ioperm },
#endif
#ifdef SYS_create_module
		{ "create_module",	SYS_create_module },
		{ "init_module",	SYS_init_module },
		{ "delete_module",	SYS_delete_module },
		{ "get_kernel_syms",	SYS_get_kernel_syms },
		{ "query_module",	SYS_query_module },
#endif
		{ "quotactl",		SYS_quotactl },
#ifdef SYS_nfsservctl
		{ "nfsservctl",		SYS_nfsservctl },
#endif
#ifdef SYS_getpmsg
		{ "getpmsg",		SYS_getpmsg },
		{ "putpmsg",		SYS_putpmsg },
#endif
#ifdef SYS_afs_syscall
		{ "afs_syscall",	SYS_afs_syscall },
#endif
#ifdef SYS_tuxcall // linux i386 misses this
		{ "tuxcall",		SYS_tuxcall },
#endif
#ifdef SYS_security // linux i386 misses this
		{ "security",		SYS_security },
#endif
		{ "gettid",		SYS_gettid },
#ifdef SYS_readahead
		{ "readahead",		SYS_readahead },
#endif
		{ "setxattr",		SYS_setxattr },
		{ "getxattr",		SYS_getxattr },
		{ "listxattr",		SYS_listxattr },
		{ "removexattr",	SYS_removexattr },
		{ "fgetxattr",		SYS_fgetxattr },
		{ "fsetxattr",		SYS_fsetxattr },
		{ "flistxattr",		SYS_flistxattr },
		{ "fremovexattr",	SYS_fremovexattr },
#ifdef SYS_lsetxattr
		{ "lsetxattr",		SYS_lsetxattr },
		{ "lgetxattr",		SYS_lgetxattr },
		{ "llistxattr",		SYS_llistxattr },
		{ "lremovexattr",	SYS_lremovexattr },
#endif
#ifdef SYS_tkill
		{ "tkill",		SYS_tkill },
#endif
#ifdef SYS_time
		{ "time",		SYS_time },
#endif
#ifdef SYS_futex
		{ "futex",		SYS_futex },
#endif
#ifdef SYS_sched_setaffinity
		{ "sched_setaffinity",	SYS_sched_setaffinity },
		{ "sched_getaffinity",	SYS_sched_getaffinity },
#endif
#ifdef SYS_set_thread_area
		{ "set_thread_area",	SYS_set_thread_area },
		{ "get_thread_area",	SYS_get_thread_area },
#endif
#ifdef SYS_io_setup
		{ "io_setup",		SYS_io_setup },
		{ "io_destroy",		SYS_io_destroy },
		{ "io_getevents",	SYS_io_getevents },
		{ "io_submit",		SYS_io_submit },
		{ "io_cancel",		SYS_io_cancel },
#endif
#ifdef SYS_lookup_dcookie
		{ "lookup_dcookie",	SYS_lookup_dcookie },
#endif
#ifdef SYS_epoll_create
		{ "epoll_create",	SYS_epoll_create },
#endif
#ifdef SYS_epoll_ctl_old // linux i386 misses this
		{ "epoll_ctl_old",	SYS_epoll_ctl_old },
		{ "epoll_wait_old",	SYS_epoll_wait_old },
#endif
#ifdef SYS_remap_file_pages
		{ "remap_file_pages",	SYS_remap_file_pages },
#endif
#ifdef SYS_getdents64
		{ "getdents64",		SYS_getdents64 },
#endif
#ifdef SYS_set_tid_address
		{ "set_tid_address",	SYS_set_tid_address },
#endif
#ifdef SYS_restart_syscall
		{ "restart_syscall",	SYS_restart_syscall },
#endif
#ifdef SYS_semtimedop
		{ "semtimedop",		SYS_semtimedop },
#endif
#ifdef SYS_fadvise64
		{ "fadvise64",		SYS_fadvise64 },
#endif
#ifdef SYS_timer_create
		{ "timer_create",	SYS_timer_create },
		{ "timer_settime",	SYS_timer_settime },
		{ "timer_gettime",	SYS_timer_gettime },
		{ "timer_getoverrun",	SYS_timer_getoverrun },
		{ "timer_delete",	SYS_timer_delete },
#endif
#ifdef SYS_clock
		{ "clock_settime",	SYS_clock_settime },
		{ "clock_gettime",	SYS_clock_gettime },
		{ "clock_getres",	SYS_clock_getres },
		{ "clock_nanosleep",	SYS_clock_nanosleep },
#endif
#ifdef SYS_exit_group
		{ "exit_group",		SYS_exit_group },
#endif
#ifdef SYS_epoll_wait
		{ "epoll_wait",		SYS_epoll_wait },
		{ "epoll_ctl",		SYS_epoll_ctl },
#endif
#ifdef SYS_tgkill
		{ "tgkill",		SYS_tgkill },
#endif
		{ "utimes",		SYS_utimes },
#ifdef SYS_vserver
		{ "vserver",		SYS_vserver },
#endif
#ifdef SYS_mbind
		{ "mbind",		SYS_mbind },
#endif
#ifdef SYS_set_mempolicy
		{ "set_mempolicy",	SYS_set_mempolicy },
		{ "get_mempolicy",	SYS_get_mempolicy },
#endif
#ifdef SYS_mq_open // Darwin misses these
		{ "mq_open",		SYS_mq_open },
		{ "mq_unlink",		SYS_mq_unlink },
		{ "mq_timedsend",	SYS_mq_timedsend },
		{ "mq_timedreceive",	SYS_mq_timedreceive },
		{ "mq_notify",		SYS_mq_notify },
		{ "mq_getsetattr",	SYS_mq_getsetattr },
#endif
#ifdef SYS_add_key
		{ "kexec_load",		SYS_kexec_load },
#endif
		{ "waitid",		SYS_waitid },
#ifdef SYS_add_key
		{ "add_key",		SYS_add_key },
		{ "request_key",	SYS_request_key },
		{ "keyctl",		SYS_keyctl },
#endif
#ifdef SYS_ioprio_set
		{ "ioprio_set",		SYS_ioprio_set },
		{ "ioprio_get",		SYS_ioprio_get },
#endif
#ifdef SYS_inotify_init
		{ "inotify_init",	SYS_inotify_init },
		{ "inotify_add_watch",	SYS_inotify_add_watch },
		{ "inotify_rm_watch",	SYS_inotify_rm_watch },
#endif
#ifdef SYS_migrate_pages // Darwin misses these
		{ "migrate_pages",	SYS_migrate_pages },
#endif
#ifdef SYS_openat // Darwin misses these
		{ "openat",		SYS_openat },
		{ "mkdirat",		SYS_mkdirat },
#ifdef SYS_mknodat // disappeared from Darwin by Yosemite
		{ "mknodat",		SYS_mknodat },
#endif
		{ "fchownat",		SYS_fchownat },
#ifdef SYS_futimesat // disappeared from Darwin by Yosemite
		{ "futimesat",		SYS_futimesat },
#endif
#ifdef SYS_newfstatat // linux x86_64, not i386 or arm
		{ "newfstatat",		SYS_newfstatat },
#endif
		{ "unlinkat",		SYS_unlinkat },
		{ "renameat",		SYS_renameat },
		{ "linkat",		SYS_linkat },
		{ "symlinkat",		SYS_symlinkat },
		{ "readlinkat",		SYS_readlinkat },
		{ "fchmodat",		SYS_fchmodat },
		{ "faccessat",		SYS_faccessat },
#endif
#ifdef SYS_pselect6
		{ "pselect6",		SYS_pselect6 },
#endif
#ifdef SYS_ppoll
		{ "ppoll",		SYS_ppoll },
#endif
#ifdef SYS_unshare
		{ "unshare",		SYS_unshare },
#endif
#ifdef SYS_set_robust_list
		{ "set_robust_list",	SYS_set_robust_list },
#endif
#ifdef SYS_get_robust_list
		{ "get_robust_list",	SYS_get_robust_list },
#endif
#ifdef SYS_splice
		{ "splice",		SYS_splice },
#endif
#ifdef SYS_tee
		{ "tee",		SYS_tee },
#endif
#ifdef SYS_sync_file_range
		{ "sync_file_range",	SYS_sync_file_range },
#endif
#ifdef SYS_vmsplice
		{ "vmsplice",		SYS_vmsplice },
#endif
#ifdef SYS_move_pages
		{ "move_pages",		SYS_move_pages },
#endif
#ifdef SYS_utimensat
		{ "utimensat",		SYS_utimensat },
#endif
#ifdef SYS_epoll_pwait
		{ "epoll_pwait",	SYS_epoll_pwait },
#endif
#ifdef SYS_signalfd
		{ "signalfd",		SYS_signalfd },
#endif
		//{ "timerfd",		SYS_timerfd },
#ifdef SYS_eventfd
		{ "eventfd",		SYS_eventfd },
#endif
#ifdef SYS_fallocate
		{ "fallocate",		SYS_fallocate },
#endif
		{ NULL },
	};

	printf("/* generated automatically by mkunix */\n\n");
	print_enum("oflags", oflags);
	print_enum("aflags", aflags);
	print_enum("seekflags", seekflags);
	print_enum("mmap_consts",mmap_consts);
	print_enum("waitflags",waitflags);
#ifdef SYS_clone // Darwin misses this
	print_enum("cloneflags",cloneflags);
#endif
	print_enum("address_families",address_families);
	print_enum("socket_type",socket_type);
	print_enum("sockopts",sockopts);
	print_enum("tcp",tcp);
	print_enum("tcp_sockopt",tcp_sockopt);
	print_enum("ioctl_nr",ioctl_nr);
	print_enum("termios_nr",termios_nr);
	print_enum("signal_nr",signal_nr);
	print_enum("mount_nr",mount_nr);
	print_enum("ptrace",ptrace);
	print_enum("syscall_nr",syscall_nr);

	return 0;

}
