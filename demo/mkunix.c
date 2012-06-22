
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#ifdef __linux__
#include <linux/fs.h>
#endif
#ifdef DARWIN
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
		printf("\t%s\t= 0x%x,\n",cursor->name,cursor->value);
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
		{ "O_TRUNC", 	O_TRUNC },	
		{ "O_APPEND", 	O_APPEND },	
		{ "O_NONBLOCK", O_NONBLOCK },	
		{ NULL },
        };

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

	struct enum_entry ioctl_nr[]={
#ifdef BLKGETSIZE // Darwin misses this
		{ "BLKGETSIZE",		BLKGETSIZE },
#endif
#ifdef DARWIN
		{ "DKIOCGETBLOCKCOUNT", DKIOCGETBLOCKCOUNT },
#endif
		{ NULL },
	};

	struct enum_entry syscall_nr[]={
		{ "read",	SYS_read },
		{ "write",	SYS_write },
		{ "open",	SYS_open },
		{ "close",	SYS_close },
		{ "stat",	SYS_stat },
		{ "fstat",	SYS_fstat },
		{ "lstat",	SYS_lstat },
		{ "poll",	SYS_poll },
		{ "lseek",	SYS_lseek },
		{ "mmap",	SYS_mmap },
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
		{ "select",		SYS_select },
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
		{ "getrlimit",		SYS_getrlimit },
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
		{ "mknodat",		SYS_mknodat },
		{ "fchownat",		SYS_fchownat },
		{ "futimesat",		SYS_futimesat },
#ifdef SYS_semtimedop // linux i386 misses this
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
	print_enum("mmap_consts",mmap_consts);
	print_enum("waitflags",waitflags);
	print_enum("address_families",address_families);
	print_enum("socket_type",socket_type);
	print_enum("ioctl_nr",ioctl_nr);
	print_enum("syscall_nr",syscall_nr);

	return 0;

}
