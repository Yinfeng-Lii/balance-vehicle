#ifndef __BSP_BLUETOOTH_H_
#define __BSP_BLUETOOTH_H_

#include "ALLHeader.h"

void bluetooth_init(void);
void UART5_Send_U8(uint8_t ch);
void UART5_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length);

#endif
