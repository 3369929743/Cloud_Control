#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "main.h"

#define TxBuffer_Size 64

typedef struct Serial_Struct{
    UART_HandleTypeDef *huart;
    uint8_t TxBuffer[TxBuffer_Size];
    __IO uint8_t isBusy;
} Serial_t;

void Serial_Init(Serial_t *Serial, UART_HandleTypeDef *huart);
void Serial_SendByte(Serial_t *Serial, uint8_t Byte);
uint8_t Serial_SendArray(Serial_t *Serial, uint8_t *Array, uint16_t Size);
uint8_t Serial_SendString(Serial_t *Serial, char *String);
void Serial_SendComplete(UART_HandleTypeDef *huart, Serial_t *Serial);
void Serial_Printf(Serial_t *Serial, char *Format, ...);
void Serial_ReceiveToIdle_IT(Serial_t *Serial, uint8_t *Buffer, uint16_t Size);
void Serial_Error_Clear(Serial_t *Serial, uint8_t* Buffer, uint16_t Size);

#endif