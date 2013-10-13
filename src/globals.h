#ifndef GLOBALS_H
#define GLOBALS_H

/* Types. */

typedef uint8_t byte;

/* Global options. */

extern bool Verbose;
extern const char* SerialPort;
extern int FastBaudRate;
extern int SlowBaudRate;
extern int MaximumPacketSize;

/* Utilities. */

extern void error(const char* message, ...);
extern void verbose(const char* message, ...);
extern void resettimer();
extern uint32_t gettime();

/* Serial port management. */

extern void logon();
extern void hexdump();

extern void sendbyte(byte c);
extern byte recvbyte();

/* Checksum-related operations. */

extern uint32_t exec_checksum(uint32_t start, uint32_t length);
extern void cmd_checksum(char** argv);

extern void exec_read(uint32_t start, uint32_t length, char* filename);
extern void cmd_read(char** argv);

#endif

