// 2016 Adam Jesionowski

#ifndef MOCKRS232_H_
#define MOCKRS232_H_

void MockRs232_Init();
void MockRs232_PushRxByte(char byte);
char MockRs232_PopTxByte();

#endif /* MOCKRS232_H_ */
