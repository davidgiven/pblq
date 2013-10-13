#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "globals.h"
#include "Packet.h"

/* --- Basic checksum ---------------------------------------------------- */

uint32_t exec_checksum(uint32_t start, uint32_t length)
{
	Packet p;

	p.request = PACKET_CHECKSUM;
	p.length = 8;
	p.setq(0, start);
	p.setq(4, length);
	p.write();
	p.read();

	if (p.gets(0) != 0x0083)
		error("Protocol error: checksum packet returned %04X, "
			"not 0083", p.gets(0));

	return p.getq(2);
};

void cmd_checksum(char** argv)
{
	const char* start = argv[0];
	const char* length = start ? argv[1] : NULL;

	if (!start || !length || argv[2])
		error("syntax error: checksum <filename> <start> <length>");

	int64_t s = strtoll(start, NULL, 0);
	int64_t l = strtoll(length, NULL, 0);
	if ((s < 0) || (l < 0) || ((s+l) > 0xFFFFFFFF))
		error("syntax error: address range out of bounds");

	printf("%08X\n", exec_checksum(s, l));
};

/* --- Read RAM ---------------------------------------------------------- */

void exec_read(uint32_t start, uint32_t length, const char* filename)
{
	FILE* fp = fopen(filename, "wb");
	if (!fp)
		error("Could not open output file: %s", strerror(errno));

	resettimer();
	for (uint32_t i=0; i<length; i++)
	{
		byte b = exec_checksum(start+i, 1);
		fputc(b, fp);

		if ((i & 0xF) == 0)
		{
			printf("\r%03d%% complete: %d bytes (%d Bps)",
				(100*i) / length,
				i, (i*1000) / gettime());
			fflush(stdout);
		}
	}
	printf("\r100\n");

	fclose(fp);
};

void cmd_read(char** argv)
{
	const char* filename = argv[0];
	const char* start = filename ? argv[1] : NULL;
	const char* length = start ? argv[2] : NULL;

	if (!filename || !start || !length || argv[3])
		error("syntax error: read <filename> <start> <length>");

	int64_t s = strtoll(start, NULL, 0);
	int64_t l = strtoll(length, NULL, 0);
	if ((s < 0) || (l < 0) || ((s+l) > 0xFFFFFFFF))
		error("syntax error: address range out of bounds");

	exec_read(s, l, filename);
};

/* --- Read NAND flash --------------------------------------------------- */

void exec_readflash(uint32_t start, uint32_t length, const char* filename)
{
	FILE* fp = fopen(filename, "wb");
	if (!fp)
		error("Could not open output file: %s", strerror(errno));

	start += FlashStartPseudoAddress;

	resettimer();
	#if 0
	for (uint32_t block=0; block<(length/512); block++)
	{
	#endif
		uint32_t block = 0;
		uint32_t address = start + block*512;
		byte previous = 0;
		for (uint32_t i=0; i<length; i++)
		{
			byte b = exec_checksum(address, i+1);
			fputc(b-previous, fp);
			previous = b;

			if ((i & 0xF) == 0)
			{
				uint32_t count = block*512 + i;
				printf("\r%03d%% complete: %d bytes (%d Bps)",
					(100*count) / length,
					count, (count*1000) / gettime());
				fflush(stdout);
			}
		}
	#if 0
	}
	#endif
	printf("\r100\n");

	fclose(fp);
};

void cmd_readflash(char** argv)
{
	const char* filename = argv[0];
	const char* start = filename ? argv[1] : NULL;
	const char* length = start ? argv[2] : NULL;

	if (!filename || !start || !length || argv[3])
		error("syntax error: readflash <filename> <start> <length>");

	int64_t s = strtoll(start, NULL, 0);
	int64_t l = strtoll(length, NULL, 0);
	if (l & 0x1FF)
		error("length must be a multiple of 512");

	if ((s < 0) || (l < 0) || (s+l >= FlashLength))
		error("syntax error: address range out of bounds");

	exec_readflash(s, l, filename);
};

