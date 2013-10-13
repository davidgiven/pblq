#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "globals.h"

static uint16_t checksum(byte* start, uint32_t length)
{
        byte checksum1 = 0;
        byte checksum2 = 0;

        while (length--) /* not sure about this */
        {
                byte b = *start++;
                checksum1 += b;
                checksum2 += checksum1;
        }

        return (checksum1<<8) | (checksum2);
};

void cmd_bless(char** argv)
{
	char* filename = argv[0];
	if (!filename || argv[1])
		error("syntax error: bless <filename>");

	int fd = open(filename, O_RDWR);
	if (fd == -1)
		error("unable to open file: %s", strerror(errno));

	/* Calculate the length of the file. */

	struct stat st;
	fstat(fd, &st);
	uint32_t length = st.st_size;

	/* Map the file into memory. */

	byte* data = (byte*) mmap(NULL, length, PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, 0);
	if (data == MAP_FAILED)
		error("could not load file: %s", strerror(errno));

	/* Calculate the checksum. */

	uint32_t headerlength = data[8] |
		(data[9] << 8) |
		(data[10] << 16) |
		(data[11] << 24);
	verbose("Chunk length is %08X\n", headerlength);

	/* It's late, and I can't do the maths to ensure the fixup byte is
	 * correct right now. So we'll brute force it. */

	uint16_t d = 0xFFFF;
	do 
	{
		d++;
		data[4] = d;
		data[5] = d>>8;
		if (checksum(data, headerlength) == 0)
		{
			verbose("Fixup word is %04X\n", d);
			break;
		}
	}
	while (d < 0xFFFF);

	munmap(data, length);
	close(fd);
}

