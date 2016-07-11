// 2016 Adam Jesionowski

#include "Helper.h"

uint32_t CharArrayToUint32(char* value)
{
	// TODO: #ifdef BIG_ENDIAN

	uint32_t ret = (value[0] & 0xFF) | ((value[1] & 0xFF) << 8) | ((value[2] & 0xFF) << 16) | ((value[3] & 0xFF) << 24);

	return ret;
}

void Uint32ToCharArray(uint32_t value, char* arr)
{
	// TODO: #ifdef BIG_ENDIAN

	arr[0] = value & 0xFF;
	arr[1]  = (value >>  8) & 0xFF;
	arr[2]  = (value >> 16) & 0xFF;
	arr[3]  = (value >> 24) & 0xFF;
}

// ALL USES OF THIS SHOULD BE CHECKED TO ENSURE LEN(str) == 8
void SetError(char* error, const char* str)
{
	uint32_t i;
	for(i = 0; i < 8; i++)
	{
		error[i] = str[i];
	}
}
