#ifndef __K230_H__
#define __K230_H__

#include "main.h"

#define ERROR_OFFSET_X 75
#define ERROR_OFFSET_Y 40

void K230_Init(void);
uint8_t Error_Update(void);
uint8_t K230_GetFlag(void);
void K230_Tick(uint16_t Size);
int16_t K230_GetError_x(void);
int16_t K230_GetError_y(void);
void K230_Error_Handler(void);

#endif
