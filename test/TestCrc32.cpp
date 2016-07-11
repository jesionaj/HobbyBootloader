// 2016 Adam Jesionowski

#include <stdlib.h>
#include "CppUTest/TestHarness.h"

extern "C" {
#include "Crc32.h"
}


TEST_GROUP(Crc32)
{

	void setup()
	{
	}

	void teardown()
	{
	}

};


TEST(Crc32, ZeroLength)
{
	LONGS_EQUAL(0, DoCRC(NULL, 0));
}

TEST(Crc32, OneByte)
{
	uint32_t message = 0x1234ABCD;
	uint32_t result = DoCRC((void*)&message, 4);

	LONGS_EQUAL(0xa697efb2, result);
}
