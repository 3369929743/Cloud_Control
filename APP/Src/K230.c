#include "K230.h"

#define K230_UART USART1
#define K230_RX_BUFFER_SIZE 12

typedef struct {
  uint8_t BufferA[K230_RX_BUFFER_SIZE];
  uint8_t BufferB[K230_RX_BUFFER_SIZE];
  uint8_t Size;
  uint8_t* Processptr;
  uint8_t* Readptr;
  __IO uint8_t BufferFlag;  //数据就绪标志
  int16_t Error_x;
  int16_t Error_y;
} K230_BUFF_t;

static K230_BUFF_t K230_Buffer ;
extern UART_HandleTypeDef huart1;

void K230_Init(void)
{
    K230_Buffer.Processptr = K230_Buffer.BufferA;
    K230_Buffer.Readptr = K230_Buffer.BufferB;
    K230_Buffer.BufferFlag = 0;
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, K230_Buffer.Processptr, K230_RX_BUFFER_SIZE); // 启动接收中断
}

uint8_t Error_Update(void)
{
    if (K230_Buffer.Size == 6) {
        if(K230_Buffer.Readptr[0] == 0xAA && K230_Buffer.Readptr[1] == 0x55){
            K230_Buffer.Error_x = (int16_t)(K230_Buffer.Readptr[3] << 8 | K230_Buffer.Readptr[2]);
            K230_Buffer.Error_y = (int16_t)(K230_Buffer.Readptr[5] << 8 | K230_Buffer.Readptr[4]);
            K230_Buffer.Error_x += ERROR_OFFSET_X;
            K230_Buffer.Error_y += ERROR_OFFSET_Y;
            return 1;
        }
    }
    return 0;
}

uint8_t K230_GetFlag(void)
{
    uint8_t Temp = K230_Buffer.BufferFlag;
    K230_Buffer.BufferFlag = 0;
    return Temp;
}

void K230_Tick(uint16_t Size)
{
    K230_Buffer.Size = Size; // 更新接收到的数据大小
    // 处理接收到的数据
    K230_Buffer.Readptr = K230_Buffer.Processptr; // 更新读取指针
    if (K230_Buffer.Processptr == K230_Buffer.BufferA) {
        K230_Buffer.Processptr = K230_Buffer.BufferB;
    }
    else {
        K230_Buffer.Processptr = K230_Buffer.BufferA;
    }
    K230_Buffer.BufferFlag = 1; // 设置数据就绪标志
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, K230_Buffer.Processptr, K230_RX_BUFFER_SIZE); // 启动接收中断
}

int16_t K230_GetError_x(void)
{
    return K230_Buffer.Error_x;
}
int16_t K230_GetError_y(void)
{
    return K230_Buffer.Error_y;
}

void K230_Error_Handler(void)
{
    (void)K230_UART->SR;
    (void)K230_UART->DR;
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, K230_Buffer.Processptr, K230_RX_BUFFER_SIZE); // 启动接收中断
}
