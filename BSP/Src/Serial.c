#include "Serial.h"
#include "string.h"
#include <stdint.h>
#include "stdarg.h"
#include "stdio.h"
#include "usart.h"

static UART_HandleTypeDef* const Serial_Map[] = {
    NULL,
    &huart1,
    &huart2,
    &huart3,
};

#define SERIAL_MAX_NUM (sizeof(Serial_Map) / sizeof(Serial_Map[0]))

static Serial_t *Serial_Instance[SERIAL_MAX_NUM] = {NULL};    

/**
 * @brief 根据UART句柄获取对应的串口对象实例
 * @param huart UART句柄指针
 * @return Serial_t* 返回对应的串口对象指针，如果未找到则返回NULL
 * @note 该函数通过比较UART外设实例地址来查找已注册的串口对象，
 *       支持USART1、USART2、USART3三个串口实例
 */
static inline Serial_t *Serial_GetInstance(UART_HandleTypeDef *huart)
{
    switch((uint32_t)huart->Instance)
    {
        case (uint32_t)USART1:
            return Serial_Instance[1];
        case (uint32_t)USART2:
            return Serial_Instance[2];
        case (uint32_t)USART3:
            return Serial_Instance[3];
        default:
            return NULL;
    }
}

/**
 * @brief 初始化串口对象
 * @param Serial 串口对象指针
 * @param Serial_Num 串口编号
 * @return uint8_t 返回0表示初始化成功，返回1表示参数错误（编号超出范围或为0）
 * @note 该函数将指定编号的UART句柄映射到串口对象，初始化状态标志和回调函数，
 *       并将串口对象注册到实例数组中
 */
uint8_t Serial_Init(Serial_t *Serial, uint8_t Serial_Num)
{
    if(Serial_Num >= SERIAL_MAX_NUM || Serial_Num == 0)
    {
        return 1;
    }
    Serial->huart = Serial_Map[Serial_Num];
    Serial->isBusy = 0;
    Serial_Instance[Serial_Num] = Serial;
    Serial->RxCallback = NULL;
    Serial->TxCallback = NULL;
    Serial->ErrorCallback = NULL;
    return 0;
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
    if(Size >= TxBuffer_Size)
    {
        Size = TxBuffer_Size - 1;
    }
    Serial_Start_Send(Serial, Size);
    va_end(Args);
}

/**
 * @brief 启动串口空闲中断接收模式
 * @param Serial 串口对象指针
 * @param Buffer 接收数据缓冲区指针
 * @param Size 接收缓冲区大小（字节数）
 * @note 该函数配置串口以空闲中断方式接收数据，当检测到总线空闲或缓冲区满时触发接收完成事件；
 *       调用前需确保Serial不为NULL，否则直接返回
 */
void Serial_ReceiveToIdle_IT(Serial_t *Serial, uint8_t *Buffer, uint16_t Size)
{
    if(Serial == NULL) return;
    Serial->pRxBuff = Buffer;
    Serial->RxSize = Size;
    HAL_UARTEx_ReceiveToIdle_IT(Serial->huart, Buffer, Size);
}

/**
 * @brief 清除串口错误标志并重新启动接收
 * @param Serial 串口对象指针
 * @note 该函数通过读取状态寄存器(SR)和数据寄存器(DR)来清除串口错误标志；
 *       清除错误后，如果接收缓冲区有效，则自动重新启动空闲中断接收
 */
static void Serial_Error_Clear(Serial_t *Serial)
{
    if(Serial == NULL) return;
    
    __HAL_UART_CLEAR_FEFLAG(Serial->huart);
    __HAL_UART_CLEAR_PEFLAG(Serial->huart);
    __HAL_UART_CLEAR_NEFLAG(Serial->huart);
    __HAL_UART_CLEAR_OREFLAG(Serial->huart);

    if(Serial->pRxBuff != NULL){
        Serial_ReceiveToIdle_IT(Serial, Serial->pRxBuff, Serial->RxSize); // 启动接收中断
    }
}

/**
 * @brief UART接收完成回调函数（HAL库中断回调）
 * @param huart UART句柄指针
 * @note 该函数由HAL库在UART接收完成时自动调用；
 *       函数会获取对应的串口对象实例，如果注册了接收回调函数，则调用用户回调并传入参数1表示接收完成
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  Serial_t *Serial = Serial_GetInstance(huart);
  if(Serial && Serial->RxCallback)
  {
    Serial->RxCallback(Serial, 1);
  }
}

/**
 * @brief UART接收事件回调函数（HAL库空闲中断回调）
 * @param huart UART句柄指针
 * @param Size 实际接收到的数据长度（字节数）
 * @note 该函数由HAL库在UART空闲中断或接收事件发生时自动调用；
 *       函数会获取对应的串口对象实例，如果注册了接收回调函数，则调用用户回调并传入实际接收的数据长度
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  Serial_t *Serial = Serial_GetInstance(huart);
  if(Serial && Serial->RxCallback)
  {
    Serial->RxCallback(Serial, Size);
  }
}

/**
 * @brief UART发送完成回调函数（HAL库中断回调）
 * @param huart UART句柄指针
 * @note 该函数由HAL库在UART发送完成时自动调用；
 *       函数会将isBusy标志清零表示串口空闲，如果注册了发送回调函数，则调用用户回调
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  Serial_t *Serial = Serial_GetInstance(huart);
  if(Serial)
  {
    Serial->isBusy = 0;
    if(Serial->TxCallback)
    {
      Serial->TxCallback(Serial);
    }
  }
}

/**
 * @brief UART错误回调函数（HAL库中断回调）
 * @param huart UART句柄指针
 * @note 该函数由HAL库在UART发生错误时自动调用；
 *       函数会先清除错误标志并重新启动接收，如果注册了错误回调函数，则调用用户回调
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  Serial_t *Serial = Serial_GetInstance(huart);
  if(Serial)
  {
    Serial_Error_Clear(Serial);
    if(Serial->ErrorCallback)
    {
        Serial->ErrorCallback(Serial);
    }
  }
}