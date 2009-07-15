#ifndef _BISONFLAW_SYS_H_
#define _BISONFLAW_SYS_H_
#define _GNU_SOURCE
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
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <signal.h>

//#include <valgrind/memcheck.h>

typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef int64_t		s64;
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef u64 Imm;

#define NORETURN	__attribute__((noreturn))
#define PAGESZ 4096
#define PAGEUP(sz)   (((sz)+PAGESZ-1)&~(PAGESZ-1))

#endif /* _BISONFLAW_SYS_H_ */
