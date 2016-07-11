// 2016 Adam Jesionowski

#ifndef INC_RS232_H_
#define INC_RS232_H_

#include <stdint.h>
#include <stdbool.h>

char Rs232GetByte();
bool Rs232ByteAvailable();
void Rs232SendByte(char value);


#endif
