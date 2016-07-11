// 2016 Adam Jesionowski
#include "CppUTest/TestHarness.h"

extern "C" {
#include "Helper.h"
}

TEST_GROUP(Helper)
{

	void setup()
	{

	}

	void teardown()
	{
	}

};

TEST(Helper, Uint32ToCharArray)
{
	uint32_t value = 0x1234ABCD;
	char array[4];

	Uint32ToCharArray(value, array);

	BYTES_EQUAL(0xCD, array[0]);
	BYTES_EQUAL(0xAB, array[1]);
	BYTES_EQUAL(0x34, array[2]);
	BYTES_EQUAL(0x12, array[3]);
}

TEST(Helper, CharArrayToUint32)
{
	uint32_t value;
	char array[4] = {0x19, 0x28, 0x37, 0x46};

	value = CharArrayToUint32(array);

	LONGS_EQUAL(0x46372819, value);
}
