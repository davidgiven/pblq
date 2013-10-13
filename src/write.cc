#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "globals.h"
#include "Packet.h"

/* --- Write RAM --------------------------------------------------------- */

void exec_write(const char* filename, uint32_t start)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp)
		error("Could not open output file: %s", strerror(errno));

	resettimer();
	Packet p;
	p.request = PACKET_WRITE;
	
	uint32_t count = 0;
	for (;;)
	{
		int i = fread(p.data+6, 1, MaximumPacketSize-8, fp);
		if (i == 0)
			break;

		p.length = 6 + i;
		p.setq(0, count+start);
		p.sets(4, i);
		p.write();
		p.read();
		p.checkresponse(0x0085);

		count += i;

		printf("\r%d bytes (%d Bps)",
			count, (count*1000) / gettime());
		fflush(stdout);
	}
	putchar('\n');

	fclose(fp);
};

void cmd_write(char** argv)
{
	const char* filename = argv[0];
	const char* start = filename ? argv[1] : NULL;

	if (!filename || !start || argv[2])
		error("syntax error: write <filename> <start>");

	int64_t s = strtoll(start, NULL, 0);
	if ((s < 0) || (s > 0xFFFFFFFF))
		error("syntax error: address range out of bounds");

	exec_write(filename, s);
};

/* --- Write flash (maybe) ----------------------------------------------- */

void exec_writeflash(const char* filename, uint32_t start)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp)
		error("Could not open output file: %s", strerror(errno));

	Packet p;
	
	resettimer();
	uint32_t count = 0;
	byte buffer[8192];
	for (;;)
	{
		int blocklen = fread(buffer, 1, 8192, fp);
		if (blocklen == 0)
			break;
		memset(buffer+blocklen, 0, 8192-blocklen);

		p.request = PACKET_ERASEFLASH;
		p.length = 10;
		p.setq(0, start+count);
		p.setq(4, 1);
		p.sets(8, 1);
		p.write();
		p.read();
		p.checkresponse(0x0086);

		int i = 0;
		for (;;)
		{
			int packetsize = (MaximumPacketSize-16) <? (8192-i);
			if (packetsize <= 0)
				break;

			p.request = PACKET_WRITEFLASH;
			p.length = 8 + packetsize;
			p.setq(0, FlashStartPseudoAddress+start+count+i);
			p.sets(4, 1);
			p.sets(6, packetsize);
			memcpy(p.data+8, buffer+i, packetsize);
			p.write();
			p.read();
			p.checkresponse(0x008E);

			i += packetsize;

			printf("\r%d bytes (%d Bps)",
				count+i, ((count+i)*1000) / gettime());
			fflush(stdout);
		}

		count += blocklen;
	}
	putchar('\n');

	fclose(fp);
};

void cmd_writeflash(char** argv)
{
	const char* filename = argv[0];
	const char* start = filename ? argv[1] : NULL;

	if (!filename || !start || argv[2])
		error("syntax error: write <filename> <start>");

	int64_t s = strtoll(start, NULL, 0);
	if ((s < 0) || (s > 0xFFFFFFFF))
		error("syntax error: address range out of bounds");

	exec_writeflash(filename, s);
};
