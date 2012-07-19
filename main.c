/*
 * Licensed under the GPL v2 or later, see the file LICENSE in this tarball.
 *  Copyright (C) 2000, Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *  Copyright (C) 2008, BusyBox Team. -solar 4/26/08
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

struct PADCONF
{
	unsigned short MUXMODE:3;
	unsigned short PULLUDENABLE:1;
	unsigned short PULLTYPESELECT:1;
	unsigned short RESERVED:3;
	unsigned short INPUTENABLE:1;
	unsigned short OFFENABLE:1;
	unsigned short OFFOUTENABLE:1;
	unsigned short OFFOUTVALUE:1;
	unsigned short OFFPULLUDENABLE:1;
	unsigned short OFFPULLTYPESELECT:1;
	unsigned short WAKEUPENABLE:1;
	unsigned short WAKEUPEVENT:1;
};

int printpadconf(const unsigned short * buf)
{
	struct PADCONF *padconf = (struct PADCONF *)buf;
    printf("MUXMODE %d, PULLUDENABLE %d, PULLTYPESELECT %d, RESERVED %d, INPUTENABLE %d, OFFENABLE %d, OFFOUTENABLE %d, OFFOUTVALUE %d, OFFPULLUDENABLE %d, OFFPULLTYPESELECT %d, WAKEUPENABLE %d, WAKEUPEVENT%d \n",
    	padconf->MUXMODE,
        padconf->PULLUDENABLE,
        padconf->PULLTYPESELECT,
        padconf->RESERVED,
        padconf->INPUTENABLE,
        padconf->OFFENABLE,
        padconf->OFFOUTENABLE,
        padconf->OFFOUTVALUE,
        padconf->OFFPULLUDENABLE,
        padconf->OFFPULLTYPESELECT,
        padconf->WAKEUPENABLE,
        padconf->WAKEUPEVENT);
    return 0;
}

int dumpmem(off_t start, off_t end) {
	void *map_base, *virt_addr;
	uint64_t read_result;
	uint64_t writeval = writeval; /* for compiler */
	off_t target;
	unsigned page_size, mapped_size, offset_in_page;
	int fd;
	unsigned width = 8 * sizeof(int);

	printf("start dump mem\n");
	
	fd = open("/dev/mem", (O_RDONLY | O_SYNC));
		
	for ( target = start; target<=end; target+=4)
	{
		mapped_size = page_size = getpagesize();
		offset_in_page = (unsigned)target & (page_size - 1);
		if (offset_in_page + width > page_size) {
			/* This access spans pages.
			* Must map two pages to make it possible: */
			mapped_size *= 2;
		}
		map_base = mmap(NULL,
				mapped_size,
				PROT_READ,
				MAP_SHARED,
				fd,
				target& ~(off_t)(page_size - 1));
		if (map_base == MAP_FAILED)
		{
		    printf("memmap failed\n");
		    return 0;
		}

			printf("offset_in_page %x \n",offset_in_page);
			//fwrite((char*)map_base + offset_in_page, 4,1,p);
            printpadconf((unsigned short*)((char*)map_base + offset_in_page));
            printpadconf((unsigned short*)((char*)map_base + offset_in_page + 2));


		if (munmap(map_base, mapped_size) == -1)
		{
		  printf("munmap failed\n");
		  return 0;
		}
	}
				
		close(fd);
    return 0;
}

int dumpfile(const char* path,off_t start, off_t end)
{
	FILE* p = NULL;
    p = fopen(path, "rb");
    unsigned short padconf = 0;
    if (p)
    {
    	int offset = 0;
    	while(feof(p) == 0)
        {
        	fread(&padconf, sizeof(padconf), 1, p);
        	printf("offset_in_page %x \n",start - 0x48002000 + offset);
        	printpadconf(&padconf);
            fread(&padconf, sizeof(padconf), 1, p);
        	printpadconf(&padconf);
            offset += 4;
        }
    }
    fclose(p);
}

int main(int argc, char **argv)
{
#ifdef __ARM
	dumpmem(0x48002030, 0x48002264);
	dumpmem(0x480025D8, 0x480025F8);
	dumpmem(0x48002A00, 0x48002A24);
#else
	dumpfile("./dump2030_2264.raw",0x48002030, 0x48002264);
	dumpfile("./dump25D8_25F8.raw",0x480025D8, 0x480025F8);
	dumpfile("./dump2A00_2A24.raw",0x48002A00, 0x48002A24);
#endif
	return 0;
}
