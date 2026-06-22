#include "Serial.h"
#include "string.h"
#include <stdint.h>
#include "stdarg.h"
#include "stdio.h"

/**
 * @brief 初始化串口对象
 * @param Serial 串口对象指针
 * @param huart UART句柄指针，指向已配置的UART外设句柄
 * @note 该函数将UART句柄绑定到串口对象，并将isBusy标志清零表示串口空闲
 */
void Serial_Init(Serial_t *Serial, UART_HandleTypeDef *huart)
{
    Serial->huart = huart;
    Serial->isBusy = 0;
}

/**
 * @brief 启动串口发送操作（内部辅助函数）
 * @param Serial 串口对象指针
 * @param Size 要发送的数据长度（字节数）
 * @note 该函数将isBusy标志置1，并调用HAL库中断发送函数启动数据传输
 */
static inline void Serial_Start_Send(Serial_t *Serial, uint16_t Size)
{
    Serial->isBusy = 1;
    HAL_UART_Transmit_IT(Serial->huart, Serial->TxBuffer, Size);
}

/**
 * @brief 通过串口发送单个字节
 * @param Serial 串口对象指针
 * @param Byte 待发送的字节数据
 * @note 发送前会等待串口空闲，然后将字节复制到TxBuffer并通过中断方式发送
 */
void Serial_SendByte(Serial_t *Serial, uint8_t Byte)
{
    while(Serial->isBusy);
    memcpy(Serial->TxBuffer, &Byte, 1);
    Serial_Start_Send(Serial, 1);
}

/**
 * @brief 通过串口发送字节数组
 * @param Serial 串口对象指针
 * @param Array 待发送的数据数组指针
 * @param Size 要发送的数据长度（字节数）
 * @return uint8_t 返回0表示发送成功，返回1表示数据长度超出缓冲区大小限制
 * @note 数据会先复制到内部TxBuffer，然后通过中断方式发送；发送时会将isBusy标志置1
 */
uint8_t Serial_SendArray(Serial_t *Serial, uint8_t *Array, uint16_t Size)
{
    if(Size > TxBuffer_Size)
    {
        Size = TxBuffer_Size;
        return 1;
    }
    while(Serial->isBusy);
    memcpy(Serial->TxBuffer, Array, Size);
    Serial_Start_Send(Serial, Size);
    return 0;
}

/**
 * @brief 通过串口发送字符串
 * @param Serial 串口对象指针
 * @param String 待发送的字符串指针
 * @return uint8_t 返回0表示发送成功，返回1表示字符串长度超出缓冲区大小限制
 * @note 数据会先复制到内部TxBuffer，然后通过中断方式发送；发送时会将isBusy标志置1
 */
uint8_t Serial_SendString(Serial_t *Serial, char *String)
{
    uint16_t Size = strlen(String);
    if(Size > TxBuffer_Size)
    {
        return 1;
    }
    while(Serial->isBusy);
    memcpy(Serial->TxBuffer, String, Size);
    Serial_Start_Send(Serial, Size);
    return 0;
}

/**
 * @brief 串口发送完成回调处理函数
 * @param huart UART句柄指针，用于匹配对应的串口外设
 * @param Serial 串口对象指针
 * @note 当UART中断发送完成时调用，若句柄匹配则将isBusy标志清零，表示串口恢复空闲状态
 */
void Serial_SendComplete(UART_HandleTypeDef *huart, Serial_t *Serial)
{
    if(huart == Serial->huart)
    {
        Serial->isBusy = 0;
    }
}

/**
 * @brief 通过串口发送格式化字符串（类似printf）
 * @param Serial 串口对象指针
 * @param Format 格式化字符串指针，支持printf风格的格式说明符
 * @param ... 可变参数列表，对应格式化字符串中的占位符
 * @note 发送前会等待串口空闲，使用vsnprintf将格式化数据写入TxBuffer，
 *       若格式化后的数据长度超出缓冲区大小则截断，然后通过中断方式发送
 */
void Serial_Printf(Serial_t *Serial, char *Format, ...)
{
    while(Serial->isBusy);
    va_list Args;
    va_start(Args, Format);
    uint16_t Size = vsnprintf((char *)Serial->TxBuffer, (int)TxBuffer_Size, Format, Args);
    if(Size > TxBuffer_Size)
    {
        Size = TxBuffer_Size;
    }
    Serial_Start_Send(Serial, Size);
    va_end(Args);
}

/**
 * @brief  启动串口空闲中断接收
 * @param  Serial: 串口句柄指针
 * @param  Buffer: 接收数据缓冲区指针
 * @param  Size: 接收数据缓冲区大小
 * @retval 无
 */
void Serial_ReceiveToIdle_IT(Serial_t *Serial, uint8_t *Buffer, uint16_t Size)
{
    HAL_UARTEx_ReceiveToIdle_IT(Serial->huart, Buffer, Size);
}

/**
 * @brief  清除串口错误标志并重新启动接收
 * @param  Serial: 串口句柄指针
 * @param  Buffer: 接收数据缓冲区指针
 * @param  Size: 接收数据缓冲区大小
 * @retval 无
 */
void Serial_Error_Clear(Serial_t *Serial, uint8_t* Buffer, uint16_t Size)
{
    (void)Serial->huart->Instance->SR;  // 读取状态寄存器清除错误标志
    (void)Serial->huart->Instance->DR;  // 读取数据寄存器清除接收数据
    Serial_ReceiveToIdle_IT(Serial, Buffer, Size); // 启动接收中断
}