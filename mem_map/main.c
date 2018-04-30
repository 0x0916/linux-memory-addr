#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	unsigned long tmp;
	int fd = -1, len = 0;
	char buf[4096];

	tmp = 0x12345678deadbeaf;
	printf("tmp variable address: 0x%08lX\n", &tmp);

	if ((fd = open("/proc/sys_reg", O_RDONLY)) == -1) {
		fprintf(stderr, "Open /proc/sys_reg file failed!\n");
		fflush(NULL);
		exit(102);
	}

	if ((len = read(fd, buf, sizeof(buf) - 1)) < 0) {
		perror("/proc/sys_reg");
		fflush(NULL);
		exit(103);
	}

	buf[len] = 0;

	printf("%s", buf);

	// loop until we type Ctrl+C
	while(1);

	return 0;
}
