#ifndef GLOBALS_H
#define GLOBALS_H

#define SPEW_TRACING

/* Types. */

typedef uint8_t byte;

/* Global options. */

extern bool Verbose;
extern const char* SerialPort;
extern int FastBaudRate;
extern int SlowBaudRate;
extern int Protocol;
extern int MaximumPacketSize;
extern bool RetryConnection;
extern uint32_t FlashStartPseudoAddress;
extern uint32_t FlashLength;

/* Utilities. */

extern void error(const char* message, ...);
extern void verbose(const char* message, ...);
extern void warning(const char* message, ...);
extern void resettimer();
extern uint32_t gettime();

/* Serial port management. */

extern void logon();
extern void hexdump();

extern void sendbyte(byte c);
extern byte recvbyte();

extern void dodgyterm();

/* Writing-related operations. */

extern void exec_write(char* filename, uint32_t start);
extern void cmd_write(char** argv);

/* Checksum-related operations. */

extern uint32_t exec_checksum(uint32_t start, uint32_t length);
extern void cmd_checksum(char** argv);

extern void exec_read(uint32_t start, uint32_t length, char* filename);
extern void cmd_read(char** argv);

extern void exec_readflash(uint32_t start, uint32_t length, char* filename);
extern void cmd_readflash(char** argv);

extern void exec_writeflash(uint32_t start, uint32_t length, char* filename);
extern void cmd_writeflash(char** argv);

/* Image management */

extern void cmd_bless(char** argv);
extern void cmd_execute(char** argv);

#endif

