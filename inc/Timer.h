// 2016 Adam Jesionowski

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

#define FIVE_SECONDS 	5000
#define FIVE_HUNDRED_MS 500

uint32_t GetTimerTimeoutValue(uint32_t time);
bool IsTimedOut(uint32_t timeoutValue);

#endif
