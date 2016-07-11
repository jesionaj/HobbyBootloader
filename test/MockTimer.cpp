// 2016 Adam Jesionowski

#include "MockTimer.h"

extern "C" {
#include "Timer.h"
}

// TODO: Make sure we handle timer overrun correctly


uint32_t TimerValue;
uint32_t Interval;

void MockTimer_Init()
{
	TimerValue = 0;
	Interval = 100;
}

void MockTimer_SetInterval(uint32_t interval)
{
	Interval = interval;
}

void MockTimer_SetTimer(uint32_t time)
{
	TimerValue = time;
}

uint32_t GetTimerTimeoutValue(uint32_t time)
{
	return time + TimerValue;
}

bool IsTimedOut(uint32_t timeoutValue)
{
	TimerValue += Interval;
	if(TimerValue > timeoutValue)
	{
		return true;
	}
	else
	{
		return false;
	}
}
