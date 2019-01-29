#ifdef __GNUC__
#ifndef _BISONFLAW_SYS_H_
#define _BISONFLAW_SYS_H_
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif /* _GNU_SOURCE */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <time.h>

//#include <valgrind/memcheck.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef int64_t		s64;
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;
typedef float		f32;
typedef double		f64;

typedef u64 Imm;
typedef uintptr_t uptr;

struct Insn* record_insn(struct Insn* i);
extern int gInsnProfiling[];

#define NORETURN	__attribute__((noreturn))
#define PAGESZ 4096
#define PAGEUP(sz)   (((sz)+PAGESZ-1)&~(PAGESZ-1))

#define	SET(x)	((x)=0)
#define	USED(x)	if(x) {} else {}

#define THREADED
#define NEXTLABEL(i) goto *(record_insn(i))->go;
#define LABEL

#endif /* _BISONFLAW_SYS_H_ */

#ifdef __cplusplus
}  // extern "C"
#endif

#else /* PLAN 9 */

#define NULL ((void*)0)
#include <u.h>
#include <libc.h>

typedef char			s8;
typedef short			s16;
typedef int			s32;
typedef long long		s64;
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef char			int8_t;
typedef short			int16_t;
typedef int			int32_t;
typedef long long		int64_t;
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

typedef unsigned long size_t;
typedef uintptr uintptr_t;

typedef u64 Imm;
typedef uintptr_t uptr;

#define NORETURN

struct timeval
{
	u32 tv_sec, tv_usec;
};
char *dirname(char *path);
typedef void FILE;

#define NEXTLABEL(i) switch(record_insn(i)->kind)
#define LABEL case

#define PRIu64 "llud"

#endif /* __GNUC__ */
