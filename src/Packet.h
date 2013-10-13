#ifndef PACKET_H
#define PACKET_H

enum {
	PACKET_NOP = 0,
	PACKET_UNKNOWN_1,
	PACKET_GETVERSION,
	PACKET_CHECKSUM,
	PACKET_UNKNOWN_4,
	PACKET_WRITE,
	PACKET_ERASEFLASH,
	PACKET_UNKNOWN_7,
	PACKET_UNKNOWN_8,
	PACKET_SETBAUD,
	PACKET_UNKNOWN_10,
	PACKET_UNKNOWN_11,
	PACKET_GETMEMINFO,
	PACKET_UNKNOWN_13,
	PACKET_WRITEFLASH,
	PACKET_UNKNOWN_15,
	PACKET_UNKNOWN_16,
	PACKET_UNKNOWN_17,
	PACKET_UNKNOWN_18,
	PACKET_UNKNOWN_19,
	PACKET_UNKNOWN_20,
};

struct Packet
{
	int length;
	int request;
	bool compressed;
	byte data[64*1024];

	Packet();

	void read();
	void write();
	void dump();
	void checkresponse(uint16_t r);
	
	byte getb(int offset);
	int16_t gets(int offset);
	int32_t getq(int offset);

	void setb(int offset, byte value);
	void sets(int offset, int16_t value);
	void setq(int offset, int32_t value);
};

#endif

