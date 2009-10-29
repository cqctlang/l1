#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main()
{
	ssize_t m;
	char buf[1024];
	while(1){
		m = read(0, buf, sizeof(buf)-1);
		if(m <= 0)
			exit(0);
		buf[m] = 0;
		fprintf(stderr, "got %s\n", buf);
		write(1, "ok\n", 3);
	}
}
