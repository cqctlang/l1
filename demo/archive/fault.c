#include <stdio.h>
#include <stdint.h>

static void
fault()
{
	char *p;
	p = (char*)(uintptr_t)0xfefe0101;  /* hopefully a bad address */
	*p = 0;
}

int
main(int argc, char *argv[])
{
	fault();
	return 0;
}
