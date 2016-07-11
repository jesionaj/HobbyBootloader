// 2016 Adam Jesionowski

#ifndef MOCKMEMORY_H_
#define MOCKMEMORY_H_

#include <stdint.h>
#include <stdbool.h>

uint32_t* MockMemory_GetEraseAddress();
void MockMemory_SetEraseResponse(bool resp);


#endif /* MOCKMEMORY_H_ */
