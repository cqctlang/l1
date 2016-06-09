#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "sys.h"
#include "util.h"
#include "syscqct.h"

enum {
	Noheap = 0,
	Heapdefined = 1,
};

#pragma weak heapimage
#pragma weak endheapimage
char heapimage[1] = { Noheap };
char endheapimage[0];

void *findheapimage(u64 *len) {
	if(heapimage[0] != Heapdefined)
		return NULL;
	*len = endheapimage-heapimage;
	return heapimage;
}
