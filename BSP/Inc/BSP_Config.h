#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__

#include <stdint.h>

#define K230_OFFSET_X 75
#define K230_OFFSET_Y 45

typedef struct Serial_Struct Serial_t;

void BSP_Serial_Init(Serial_t *Serial, uint8_t Serial_Num);

#endif