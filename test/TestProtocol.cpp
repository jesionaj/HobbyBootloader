// 2016 Adam Jesionowski

#include <stdlib.h>
#include "CppUTest/TestHarness.h"
#include "MockRs232.h"
#include "MockTimer.h"
#include "MockMemory.h"

extern "C" {
#include "Protocol.h"
#include "Bootloader.h"
#include "Timer.h"
#include "Helper.h"
}

TEST_GROUP(Protocol)
{

	void setup()
	{
		MockRs232_Init();
		MockTimer_Init();
	}

	void teardown()
	{
	}

	void SendU32(uint32_t value)
	{
		char array[4];

		Uint32ToCharArray(value, array);

		for(int i = 0; i < 4; i++)
		{
			MockRs232_PushRxByte(array[i]);
		}
	}

};

TEST(Protocol, Uint32ToCharArray)
{
	uint32_t value = 0x1234ABCD;
	char array[4];

	Uint32ToCharArray(value, array);

	BYTES_EQUAL(0xCD, array[0]);
	BYTES_EQUAL(0xAB, array[1]);
	BYTES_EQUAL(0x34, array[2]);
	BYTES_EQUAL(0x12, array[3]);

	LONGS_EQUAL(value, *(uint32_t*)array); // TODO: A little iffy on this check.
}

TEST(Protocol, CharArrayToUint32)
{
	uint32_t value;
	char array[4] = {0x19, 0x28, 0x37, 0x46};

	value = CharArrayToUint32(array);

	LONGS_EQUAL(0x46372819, value);
}

TEST(Protocol, GetByteTimeout_TimesOut)
{
	// Don't push anything to TX queue, so we'll timeout
	char byte;

	bool res = GetByteTimeout(&byte, FIVE_HUNDRED_MS);

	CHECK_FALSE(res);
}

TEST(Protocol, GetByteTimeout_Good)
{
	MockRs232_PushRxByte(0xAB);
	char byte;

	bool res = GetByteTimeout(&byte, FIVE_HUNDRED_MS);

	CHECK_TRUE(res);
	BYTES_EQUAL(0xAB, byte);
}

TEST(Protocol, GetFourBytesTimeout_TimesOutNoData)
{
	// Don't push anything to TX queue, so we'll timeout
	uint32_t value;

	bool res = GetFourBytesTimeout(&value, FIVE_HUNDRED_MS);

	CHECK_FALSE(res);
}

TEST(Protocol, GetFourBytesTimeout_TimesOutTwoBytes)
{
	// Pushing half the message will mean we still time out
	MockRs232_PushRxByte(0xAB);
	MockRs232_PushRxByte(0xCD);
	uint32_t value;

	bool res = GetFourBytesTimeout(&value, FIVE_HUNDRED_MS);

	CHECK_FALSE(res);
}

TEST(Protocol, GetFourBytesTimeout_Good)
{
	MockRs232_PushRxByte(0xAB);
	MockRs232_PushRxByte(0xCD);
	MockRs232_PushRxByte(0x56);
	MockRs232_PushRxByte(0x78);
	uint32_t value;

	bool res = GetFourBytesTimeout(&value, FIVE_HUNDRED_MS);

	CHECK_TRUE(res);
	LONGS_EQUAL(0x7856CDAB, value);
}

TEST(Protocol, DoProtocol_Timeout)
{
	DoProtocol();

	char ret = MockRs232_PopTxByte();

	BYTES_EQUAL('t', ret);
}

TEST(Protocol, DoProtocol_Version)
{
	MockRs232_PushRxByte('V');
	char rcv[4];
	uint32_t rcvVersion;

	version = 0x10ABCD01;

	DoProtocol();

	for(int i = 0; i < 4; i++)
		rcv[i] = MockRs232_PopTxByte();

	rcvVersion = CharArrayToUint32(rcv);

	LONGS_EQUAL(version, rcvVersion);
}

TEST(Protocol, DoProtocol_Error)
{
	MockRs232_PushRxByte('R');
	char rcv[8];

	SetError(error, "DeadJim\0"); // Using a null makes strcmp work

	DoProtocol();

	for(int i = 0; i < 8; i++)
		rcv[i] = MockRs232_PopTxByte();

	STRCMP_EQUAL(error, rcv);
}

TEST(Protocol, DoProtocol_Boot)
{
	MockRs232_PushRxByte('B');
	char rcv;

	DoProtocol();

	rcv = MockRs232_PopTxByte();

	BYTES_EQUAL('b', rcv);
}

TEST(Protocol, DoProtocol_Erase_Timeout)
{
	// Send erase, don't send address
	MockRs232_PushRxByte('E');
	char rcv;

	DoProtocol();

	rcv = MockRs232_PopTxByte();

	BYTES_EQUAL('t', rcv);
}

TEST(Protocol, DoProtocol_Erase_Good)
{
	uint32_t* address = (uint32_t*)0x12340987;

	MockMemory_SetEraseResponse(true);
	MockRs232_PushRxByte('E');
	SendU32((uint32_t)address);

	DoProtocol();

	char rcv = MockRs232_PopTxByte();


	BYTES_EQUAL('e', rcv);
	LONGS_EQUAL(address, MockMemory_GetEraseAddress());
}

TEST(Protocol, DoProtocol_Erase_BadAddress)
{
	uint32_t* address = (uint32_t*)0x12340987;

	MockMemory_SetEraseResponse(false);

	MockRs232_PushRxByte('E');
	SendU32((uint32_t)address);

	DoProtocol();

	char rcv = MockRs232_PopTxByte();

	BYTES_EQUAL('f', rcv);
	LONGS_EQUAL(address, MockMemory_GetEraseAddress());
}

// TODO: Program routine
