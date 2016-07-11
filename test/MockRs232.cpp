// 2016 Adam Jesionowski

#include <queue>
#include <utility>

#include "MockRs232.h"

extern "C" {
#include "Rs232.h"
}

std::queue<char> rx_q;
std::queue<char> tx_q;

void MockRs232_Init()
{
	// I guess this is the best way to clear out a std::queue
	std::queue<char> empty_rx;
    std::swap( rx_q, empty_rx );

    std::queue<char> empty_tx;
    std::swap( tx_q, empty_tx );
}

void MockRs232_PushRxByte(char byte)
{
	rx_q.push(byte);
}

char MockRs232_PopTxByte()
{
	char ret = tx_q.front();
	tx_q.pop();
	return ret;
}

// Rs232.h functions
char Rs232GetByte()
{
	char ret = 0;

	if(!rx_q.empty())
	{
		ret = rx_q.front();
		rx_q.pop();
	}

	return ret;
}

void Rs232SendByte(char value)
{
	tx_q.push(value);
}


bool Rs232ByteAvailable()
{
	return !rx_q.empty();
}

