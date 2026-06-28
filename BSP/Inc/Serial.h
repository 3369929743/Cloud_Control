#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "main.h"
#include <stdint.h>

#define TxBuffer_Size 64

/**
 * @brief 串口对象结构体
 */
typedef struct Serial_Struct{
    UART_HandleTypeDef *huart;          /*!< UART句柄指针，指向底层HAL库UART外设句柄 */
    uint8_t TxBuffer[TxBuffer_Size];    /*!< 发送数据缓冲区，用于暂存待发送的数据 */
    __IO uint8_t isBusy;                /*!< 发送忙标志位，1表示正在发送中，0表示空闲 */

    uint8_t *pRxBuff;                   /*!< 接收数据缓冲区指针，指向用户提供的接收缓冲区 */
    uint16_t RxSize;                    /*!< 接收缓冲区大小（字节数） */

    void (*RxCallback)(struct Serial_Struct *Serial, uint16_t Size);   /*!< 接收完成回调函数指针，Size为实际接收到的数据长度 */
    void (*TxCallback)(struct Serial_Struct *Serial);                  /*!< 发送完成回调函数指针 */
    void (*ErrorCallback)(struct Serial_Struct *Serial);               /*!< 错误处理回调函数指针 */
} Serial_t;

uint8_t Serial_Init(Serial_t *Serial, uint8_t Serial_Num);
void Serial_SendByte(Serial_t *Serial, uint8_t Byte);
uint8_t Serial_SendArray(Serial_t *Serial, uint8_t *Array, uint16_t Size);
uint8_t Serial_SendString(Serial_t *Serial, char *String);
void Serial_Printf(Serial_t *Serial, char *Format, ...);
void Serial_ReceiveToIdle_IT(Serial_t *Serial, uint8_t *Buffer, uint16_t Size);

#endif