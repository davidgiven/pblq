#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "globals.h"
#include "Packet.h"

static uint8_t checksum;

static void outb(byte b)
{
	checksum += b;
	sendbyte(b);
}

static void outs(int16_t s)
{
	outb((s>>0) & 0xFF);
	outb((s>>8) & 0xFF);
}

static byte inb()
{
	byte b = recvbyte();
	checksum += b;
	return b;
}

static int16_t ins()
{
	return (inb()
		| (inb() << 8));
}

Packet::Packet()
{
	length = 0;
	request = 0;
	compressed = false;
}

void Packet::write()
{
	outb(0x02);       // STX prefix
	outb(0);          // not compressed

	checksum = 0;
	outs(length+2); // data length (inc. opcode)
	outs(request);  // opcode

	for (int i=0; i<length; i++)
		outb(data[i]);

	outb(checksum);
}

void Packet::read()
{
	byte b = inb();
	if (b != 0x02)
	{
		error("Protocol error: incorrect packet prefix "
			"(got %02X, should be 02)",
			b);
	}

	compressed = inb();

	checksum = 0;
	length = ins();

	for (int i=0; i<length; i++)
		data[i] = inb();

	b = recvbyte();
	if (b != checksum)
		error("Protocol error: checksum mismatch "
			"(got %02X, should be %02X)",
			b, checksum);
}

void Packet::dump()
{
	printf("%04X: ", length);
	for (int i=0; i<length; i++)
		printf("%02X ", data[i]);
	printf("\n");
};
	
void Packet::checkresponse(uint16_t r)
{
	uint16_t s = gets(0);
	if (s != r)
	{
		printf("\n");
		dump();
		error("Protocol error: received response packet %08X, "
			"but was expecting %08X", s, r);
	}
}

byte Packet::getb(int offset)
{
	return data[offset];
}

int16_t Packet::gets(int offset)
{
	return data[offset]
		| (data[offset+1] << 8);
}

int32_t Packet::getq(int offset)
{
	return data[offset]
		| (data[offset+1] << 8)
		| (data[offset+2] << 16)
		| (data[offset+3] << 24);
}

void Packet::setb(int offset, byte value)
{
	data[offset] = value;
}

void Packet::sets(int offset, int16_t value)
{
	data[offset+0] = (value>>0) & 0xFF;
	data[offset+1] = (value>>8) & 0xFF;
}

void Packet::setq(int offset, int32_t value)
{
	data[offset+0] = (value>> 0) & 0xFF;
	data[offset+1] = (value>> 8) & 0xFF;
	data[offset+2] = (value>>16) & 0xFF;
	data[offset+3] = (value>>24) & 0xFF;
}

