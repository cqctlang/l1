/*
 * Lib9 is miscellany from the Plan 9 Port C library
 */
#ifndef _LIBC_H_
#define _LIBC_H_ 1
#if defined(__cplusplus)
extern "C" {
#endif                                                                

#ifndef nil
#define	nil	((void*)0)
#endif
#define	nelem(x)	(sizeof(x)/sizeof((x)[0]))

#ifndef offsetof
#define offsetof(s, m)	(ulong)(&(((s*)0)->m))
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef long long vlong;
typedef unsigned long long uvlong;
typedef uint8_t  u8int;
typedef uint16_t u16int;
typedef uint32_t u32int;
typedef uint64_t u64int;

extern	int	dec64(uchar*, int, char*, int);
extern	int	enc64(char*, int, uchar*, int);
extern	int	dec32(uchar*, int, char*, int);
extern	int	enc32(char*, int, uchar*, int);
extern	int	dec16(uchar*, int, char*, int);
extern	int	enc16(char*, int, uchar*, int);

#if defined(__cplusplus)
}
#endif
#endif	/* _LIB9_H_ */
