#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include "globals.h"
#include "Packet.h"

bool Verbose = false;
const char* SerialPort = "/dev/ttyS0";
int FastBaudRate = 115200;
int SlowBaudRate = 9600;
int MaximumPacketSize = 0;

static int argc;
static char** argv;

static void show_help()
{
	printf(
		"pblq: Amstrad PBL boot load client.\n"
		"Usage: pblq [<options>] <command...>\n"
		"Options:\n"
		"   -h         Displays this message\n"
		"   -v         Switch on verbose mode\n"
		"   -p <port>  Sets the serial port to use\n"
		"   -s <rate>  Sets the slow baud rate\n"
		"   -f <rate>  Sets the fast baud rate\n"
		"   -m <size>  Sets the maximum packet size (0=ask PBL)\n"
		"Commands:\n"
		"   ping       Pings the device, testing communication\n"
		"   checksum <startaddress> <length>\n"
		"              Calculates the checksum of an area of memory\n"
		"   read <filename> <startaddress> <length>\n"
		"              Reads data from RAM into the specified file\n"
		"Addresses should begin 0x if you want them in hex.\n"
	);
}

static void parse_options()
{
	for (;;)
	{
		int c = getopt(argc, argv,
			":hvp:f:s:m:");
		switch (c)
		{
			case -1:
				argv += optind;
				return;

			case 'h':
				show_help();
				exit(0);

			case 'v':
				Verbose = true;
				break;

			case 'p':
				SerialPort = optarg;
				break;

			case 'f':
				FastBaudRate = atoi(optarg);
				break;

			case 's':
				SlowBaudRate = atoi(optarg);
				break;

			case 'm':
				MaximumPacketSize = atoi(optarg);
				break;

			case ':':
				error("missing option argment.");

			default:
				error("try '-h' for a usage summary.");
		}
	}
}

int main(int _argc, char* _argv[])
{
	argc = _argc;
	argv = _argv;
	parse_options();

	const char* cmd = argv[0];
	if (!cmd)
		error("nothing to do! Try '-h' for a usage summary.");
		
	logon();

	argv++;
	if (strcmp(cmd, "ping") == 0)
	{
		/* The logon's already done this. */
	}
	else if (strcmp(cmd, "checksum") == 0)
		cmd_checksum(argv);
	else if (strcmp(cmd, "read") == 0)
		cmd_read(argv);

	return 0;
};

