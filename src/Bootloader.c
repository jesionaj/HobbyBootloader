// 2016 Adam Jesionowski
// This file is still being worked on

#include "Bootloader.h"
#include "Crc32.h"
#include "Protocol.h"
#include "Rs232.h"
#include "Timer.h"
#include "Helper.h"

typedef enum {
	Check,
	NotLoaded,
	Booting,
	Communicate,
	PowerDown
} BootloaderState;

// Bootloader FSM:
// Check: Do CRC. If good -> Booting, If bad -> Not loaded
// Not loaded -> wait until communication, then -> Communicate
// Booting -> If we get communication, then -> Communicate. Otherwise -> Boot program
// Communicate -> Upon return -> Check
// PowerDown -> Go to sleep mode, then -> Check when we wake up

// Header:
// 0xABBABAAB
// Version
// Start address?
// End address
// Version

// Footer
// 0xF00F0FF0
// CRC
#define HEADER_CONSTANT 0xABBABAAB
#define FOOTER_CONSTANT 0xF00F0FF0


// Used by protocol, non-static
uint32_t version;
char error[8];

static uint32_t* header;
static uint32_t* start;
static uint32_t length;
static uint32_t* footer;
static uint32_t crc;
static const uint32_t* timer;
static uint32_t power_down_time;
static uint32_t boot_time;

// TODO: Refactor this with some of the timer changes
// TODO: Write unit tests
BootloaderState BootloaderFSM(BootloaderState state)
{
	BootloaderState next_state; // Leave like this for static analysis to catch or set to set state?
	char comByte;

	switch(state)
	{
	case Check:
		// Check that the program has been programmed correctly. If so, go to Boot. Otherwise, go to Not Loaded.

		if(*header == HEADER_CONSTANT)
		{
			// We want to read from the header, so let's set a pointer to it that we can advance
			uint32_t* read_header = header;
			// We should be pointing to start now
			read_header++;

			// Read start address, length, version, and footer address
			start = (uint32_t*)*read_header++;
			length = *read_header++;
			version = *read_header++;
			footer = (uint32_t*)*read_header++;

			crc = *(footer + 1);

			// TODO: Validate data

			if(*footer == FOOTER_CONSTANT)
			{
				uint32_t crc = DoCRC(start, length);

				if(crc == crc)
				{
					next_state = Booting;
				}
				else
				{
					SetError(error, "CrcFaild");
					next_state = NotLoaded;
				}
			}
			else
			{
				SetError(error, "BadFootr");
				next_state = NotLoaded;
			}
		}
		else
		{
			SetError(error, "BadHeadr");
			next_state = NotLoaded;
		}
		break;
	case NotLoaded:
		// Wait for 5 seconds. If no communication during that time, then power down

		if(power_down_time == 0)
		{
			power_down_time = *timer + FIVE_SECONDS;
			next_state = NotLoaded;
		}

		// Check for a message to push us into communicate
		comByte = Rs232GetByte();
		if(IsBootCommand(comByte))
		{
			next_state = Communicate;
		}
		else
		{
			// Otherwise, check if we need to power down
			if(*timer > power_down_time)
			{
				// 5 seconds have passed, power the device down
				next_state = PowerDown;
			}
			else
			{
				// Keep waiting.
				// TODO: Wait here?
				next_state = NotLoaded;
			}
		}

		break;
	case Booting:
		// Wait for 0.5 ms. If no communication during that time, boot our program.
		if(boot_time == 0)
		{
			boot_time = *timer + FIVE_HUNDRED_MS;
		}

		// Check if a service tool wants to communicate
		comByte = Rs232GetByte();
		if(IsBootCommand(comByte))
		{
			// Move to communicate state if so
			next_state = Communicate;
		}
		else
		{
			// Otherwise, check if it's time to boot
			if(boot_time > *timer)
			{
				// Point of No Return
				// Boot();
			}
			else
			{
				// Keep waiting
				// TODO: Wait here?
				next_state = Booting;
			}
		}

		break;
	case PowerDown:
		// PowerDown();

		// Once we come out of sleep, then go to check
		next_state = Check;

		break;
	case Communicate:
		DoProtocol();
		next_state = Check;

		break;
	}

	return next_state;
}



