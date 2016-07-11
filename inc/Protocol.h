// 2016 Adam Jesionowski

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <stdint.h>
#include <stdbool.h>

// TODO: Move these to a helper file?
uint32_t CharArrayToUint32(char* value);
void Uint32ToCharArray(uint32_t value, char* arr);
bool GetByteTimeout(char* value, uint32_t interval);
bool GetFourBytesTimeout(uint32_t* value, uint32_t interval);
bool IsBootCommand(char cmd);

void DoProtocol();

#endif
