#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/mman.h>
#include <sys/types.h>

#define PAGE_MAP_FILE "/proc/self/pagemap"
#define PFN_MASK ((((uint64_t)1) << 55) - 1)
#define PFN_PRESENT_FLAG  (((uint64_t)1) << 63)

/*use /page/pid/pagemap to get the relationship between user space va and pa */
int userSpaceVa2Pa(uint64_t va, uint64_t *pa)
{
	int fd;
	int page_size = getpagesize();
	uint64_t va_page_index = va / page_size;
	uint64_t pfn_pagemap_offset = va_page_index*sizeof(uint64_t);
	uint64_t pfn;
	fd  = open(PAGE_MAP_FILE, O_RDONLY);
	if(fd < 0)
	{
		printf("open page map file fail \n");
		return 1;
	}
	if(sizeof(uint64_t) != pread(fd, &pfn, sizeof(uint64_t), pfn_pagemap_offset))
	{
		printf("pread fail \n");
		return 1;
	}
	if( 0 == (pfn & PFN_PRESENT_FLAG))
	{
		printf("page is not present \n");
		return 1;
	}
	*pa = (pfn & PFN_MASK)*page_size + va % page_size;
	return 0;
}


int main()
{
	int fd;
	char *va1;
	char *va2;
	uint64_t pa1 = 0;
	uint64_t pa2 = 0;
	fd = open("mmap.txt",O_RDWR|O_CREAT);
	if(fd < 0 )
	{
		printf("open file fail \n");
		return 1;
	}
	va1 = (char*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	va2 = (char*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	printf("va1 is %p,  va2 is %p \n", va1,va2);
	*va1 = 'X'; //page fault so page present
//	*va2 = 'X';

	userSpaceVa2Pa((uint64_t)va1,&pa1);
	userSpaceVa2Pa((uint64_t)va1,&pa2);
	printf("pa1 is %p, pa2 is %p \n", pa1, pa2);
	/*so we can see the same process and differernt user space va map the same file use the same pa*/

	mprotect(va2,4096, PROT_NONE);
	*va1 = 'X';
	//*va2 = 'X';  //segmentation fault
	return 0;
	
}
