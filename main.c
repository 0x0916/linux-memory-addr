#include <stdio.h>

int main(int argc, char **argv)
{
	unsigned long tmp;
	tmp = 0x12345678;
	printf("tmp variable address: 0x%08lX\n", &tmp);
	return 0;
}
