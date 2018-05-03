//-------------------------------------------------------------------
//	showgdt.cpp
//
//	This program uses our 'dram.c' device-driver to display the 
//	current processor's Global Descriptor Table in hex format.
//
//		compile using:  $ g++ showgdt.cpp -o showgdt
//		execute using:  $ ./showgdt
//
//	NOTE: Our 'dram.ko' module must be installed in the kernel.	
//
//	programmer: ALLAN CRUSE
//	written on: 18 MAR 2006
//	change START_KERNEL_map for x86_64
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <fcntl.h>	// for open() 
#include <unistd.h>	// for read()  

#define START_KERNEL_map 0xffff880000000000

char devname[] = "/dev/dram";
unsigned short	gdtr[3];
unsigned long	gdt_virt_address;
unsigned long	gdt_phys_address;

struct desc_struct {
	unsigned short limit0;
	unsigned short  base0;
	unsigned base1: 8, type: 4, s: 1, dpl: 2, p: 1;
	unsigned limit: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
} __attribute__((packed));

int main(int argc, char **argv)
{
	int i;
	int fd;
	// use inline assembly language to get GDTR register-image
	asm(" sgdt gdtr ");

	// extract GDT virtual-address from GDTR register-image
	gdt_virt_address = *(unsigned long*)(gdtr+1);

	// compute GDT physical-address using subtraction
	gdt_phys_address = gdt_virt_address - START_KERNEL_map;

	// extract GDT segment-limit and compute descriptor count
	int	n_elts = (1 + gdtr[0])/8;

	// report the GDT virtual and physical memory-addresses 
	printf("gdt_virt_address=%08lX ", gdt_virt_address);
	printf("gdt_phys_address=%08lX ", gdt_phys_address);
	printf("gdt_limit=%04lX ", gdtr[0]);
	printf("\n\n" );

	// find, read and display the GDT's entries in device-memory
	fd = open(devname, O_RDONLY);
	if ( fd < 0 ) {
		perror(devname); 
		return -1;
	}

	lseek(fd, gdt_phys_address, SEEK_SET);
	for (i = 0; i < n_elts; i++)
	{
		printf("%04X: ", i*8);
		struct desc_struct	desc;
		read(fd, &desc, sizeof(struct desc_struct));
		printf("%016llX ", *(unsigned long long*)&desc);
		printf(" Base: 0x%016lX", (desc.base2 << 24) | (desc.base1 << 16) | desc.base0);
		printf(" Limit: 0x%05lX", (desc.limit << 16) | desc.limit0);
		printf(" Type: 0x%lX", desc.type);
		printf(" G: 0x%lX", desc.g);
		printf(" DPL: 0x%lX", desc.dpl);
		printf(" P: 0x%lX", desc.p);
		printf(" S: 0x%lX", desc.s);
		printf(" B/D: 0x%lX", desc.d);
		printf(" AVL: 0x%lX", desc.avl);
		printf("\n" );
	}
}
