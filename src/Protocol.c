// 2016 Adam Jesionowski

#include "Protocol.h"
#include "Bootloader.h"
#include "Crc32.h"
#include "Rs232.h"
#include "Timer.h"
#include "Memory.h"

#define VERSION_CMD 'V'
#define ERROR_CMD 'R'
#define BOOT_CMD 'B'
#define PROGRAM_CMD 'P'
#define ERASE_CMD 'E'
#define CHECK_CMD 'C'

#define PROGRAM_RESP 'p'
#define PROGRAM_FAILED_RESP 'q'
#define PROGRAM_SUCCEEDED_RESP 'r'
#define TIMEOUT_RESP 't'
#define ERASE_END_RESP 'e'
#define ERASE_BAD_RESP 'f'
#define BOOT_RESP 'b'
#define NOT_RECOGNIZED_RESP '?'
#define GOOD_RESP '1'
#define BAD_RESP '0'

static uint32_t in_memory_array[128];


bool IsBootCommand(char cmd)
{
	if(cmd == BOOT_CMD)
	{
		return true;
	}
	else
	{
		return false;
	}
}


// Returns true if byte available, false if timed out
bool GetByteTimeout(char* value, uint32_t interval)
{
	uint32_t timeout_value = GetTimerTimeoutValue(interval);
	bool timeout = false;

	// Wait for an RS232 byte to be available, or time out
	while(!Rs232ByteAvailable() && !timeout){
		timeout = IsTimedOut(timeout_value);
		// TODO: Consider a small sleep here.
	}

	if(!timeout)
	{
		*value = Rs232GetByte();
	}

	return !timeout;
}

bool GetFourBytesTimeout(uint32_t* value, uint32_t interval)
{
	uint32_t rcv_idx = 0;
	char rcv[4];

	while(rcv_idx < 4)
	{
		if(GetByteTimeout(&rcv[rcv_idx], interval))
		{
			rcv_idx++;
		}
		else
		{
			return false;
		}
	}

	*value = CharArrayToUint32(rcv);

	return true;
}

void DoProtocol()
{
	int i;
	char command;
	char sendUint32[4];

	uint32_t* address;		// Used when a command returns a pointer to memory. TODO: Validate pointer.
	uint32_t rcv_program;	// Used for receiving application data
	uint32_t crc;			// Used for calculating the CRC of the received row

	// Wait for 5 seconds for a command to be available. If none arrives, we're done.
	if(!GetByteTimeout(&command, FIVE_SECONDS))
	{
		Rs232SendByte(TIMEOUT_RESP);
		return;
	}

	switch(command)
	{
	case VERSION_CMD:
		Uint32ToCharArray(version, sendUint32);
		for(i = 0; i < 4; i++)
		{
			Rs232SendByte(sendUint32[i]);
		}
		break;
	case ERROR_CMD:
		for(i = 0; i < 8; i++)
		{
			Rs232SendByte(error[i]);
		}
		break;
	case BOOT_CMD:
		Rs232SendByte(BOOT_RESP);
		break;
	case PROGRAM_CMD:
		// We need to wait for four bytes
		if(GetFourBytesTimeout(&address, FIVE_HUNDRED_MS))
		{
			// We got the address, send start programming response
			Rs232SendByte(PROGRAM_RESP);

			// Alright, let's get 512 bytes!
			bool timeout = false;
			for(i = 0; i < 128; i++)
			{
				if(GetFourBytesTimeout(&rcv_program, FIVE_HUNDRED_MS))
				{
					in_memory_array[i] = rcv_program;
				}
				else
				{
					timeout = true;
					break;
				}
			}

			// Okay, now we get the CRC32 value
			if(!timeout && GetFourBytesTimeout(&crc, FIVE_HUNDRED_MS))
			{
				// Now check checksums
				uint32_t mem_crc = DoCRC((uint32_t*)&in_memory_array, 128);

				if(mem_crc == crc)
				{
					// CRC succeeded, program the application
					// Pauper memcpy
					for(i = 0; i < 128; i++)
					{
						address[i] = in_memory_array[i];
					}
					Rs232SendByte(PROGRAM_SUCCEEDED_RESP);
				}
				else
				{
					Rs232SendByte(PROGRAM_FAILED_RESP);
				}
			}
			else
			{
				Rs232SendByte(TIMEOUT_RESP);
			}
		}
		break;
	case ERASE_CMD:
		// We need to wait for four bytes
		if(GetFourBytesTimeout((uint32_t*)&address, FIVE_HUNDRED_MS))
		{

			if(Erase(address))
			{
				Rs232SendByte(ERASE_END_RESP);
			}
			else
			{
				Rs232SendByte(ERASE_BAD_RESP);
			}
		}
		else
		{
			Rs232SendByte(TIMEOUT_RESP);
		}

		break;
	case CHECK_CMD:
		// TODO: Not sure if we're going to do this, or just use booting as the check
		break;
	default:
		Rs232SendByte(NOT_RECOGNIZED_RESP);
		break;

	}

	return;
}

