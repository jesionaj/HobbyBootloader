// 2016 Adam Jesionowski

#include "MockMemory.h"

extern "C" {
	#include "Memory.h"
}

uint32_t* RcvAddress;
bool EraseResponse = true;

uint32_t* MockMemory_GetEraseAddress()
{
	return RcvAddress;
}

void MockMemory_SetEraseResponse(bool resp)
{
	EraseResponse = resp;
}


// Memory.h functions
bool Erase(uint32_t* address)
{
	RcvAddress = address;

	return EraseResponse;
}
