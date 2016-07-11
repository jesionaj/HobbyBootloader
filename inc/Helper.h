// 2016 Adam Jesionowski

#ifndef INC_HELPER_H_
#define INC_HELPER_H_

#include <stdint.h>
#include <stdbool.h>

uint32_t CharArrayToUint32(char* value);
void Uint32ToCharArray(uint32_t value, char* arr);
void SetError(char* error, const char* str);

#endif /* INC_HELPER_H_ */
