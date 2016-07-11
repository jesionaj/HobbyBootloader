// 2016 Adam Jesionowski

#ifndef MOCKTIMER_H_
#define MOCKTIMER_H_

#include <stdint.h>

void MockTimer_Init();
void MockTimer_SetInterval(uint32_t interval);
void MockTimer_SetTimer(uint32_t time);


#endif /* MOCKTIMER_H_ */
