#include "K230.h"

#define K230_RX_BUFFER_SIZE 12

typedef struct {
    UART_HandleTypeDef *huart;
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

/**
 * @brief 初始化K230串口接收缓冲区
 * @param huart UART句柄指针，指向已配置的UART外设句柄
 * @note 该函数配置双缓冲区（BufferA和BufferB），初始化处理指针和读取指针，
 *       清零数据就绪标志，并启动UART空闲中断接收
 */
void K230_Init(UART_HandleTypeDef *huart)
{
    K230_Buffer.huart = huart;
    K230_Buffer.Processptr = K230_Buffer.BufferA;
    K230_Buffer.Readptr = K230_Buffer.BufferB;
    K230_Buffer.BufferFlag = 0;
    HAL_UARTEx_ReceiveToIdle_IT(K230_Buffer.huart, K230_Buffer.Processptr, K230_RX_BUFFER_SIZE); // 启动接收中断
}

/**
 * @brief 更新误差数据（解析K230接收到的数据帧）
 * @return uint8_t 返回1表示误差数据更新成功，返回0表示数据无效或未更新
 * @note 数据帧格式：帧头(0xAA 0x55) + Error_x(2字节小端序) + Error_y(2字节小端序)
 *       解析后会自动加上预设的偏移量(ERROR_OFFSET_X和ERROR_OFFSET_Y)
 */
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

/**
 * @brief 获取并清除K230数据就绪标志
 * @return uint8_t 返回标志值，1表示有新数据就绪，0表示无新数据
 * @note 该函数采用"读取后清零"机制，每次调用后标志位会被自动清除
 */
uint8_t K230_GetFlag(void)
{
    uint8_t Temp = K230_Buffer.BufferFlag;
    K230_Buffer.BufferFlag = 0;
    return Temp;
}

/**
 * @brief K230串口接收事件处理函数（在中断回调中调用）
 * @param huart UART句柄指针，用于匹配对应的串口外设
 * @param Size 本次接收到的数据字节数
 * @note 该函数实现双缓冲区切换机制：更新读取指针指向刚接收完的数据，
 *       切换处理指针到另一个缓冲区，设置数据就绪标志，并重新启动空闲中断接收
 */
void K230_Tick(UART_HandleTypeDef *huart,uint8_t Size)
{
    if(huart != K230_Buffer.huart){
        return;
    }
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
    HAL_UARTEx_ReceiveToIdle_IT(K230_Buffer.huart, K230_Buffer.Processptr, K230_RX_BUFFER_SIZE); // 启动接收中断
}

/**
 * @brief 获取X轴误差值
 * @return int16_t 返回当前X轴误差值（已包含偏移量校正）
 */
int16_t K230_GetError_x(void)
{
    return K230_Buffer.Error_x;
}
/**
 * @brief 获取Y轴误差值
 * @return int16_t 返回当前Y轴误差值（已包含偏移量校正）
 */
int16_t K230_GetError_y(void)
{
    return K230_Buffer.Error_y;
}

/**
 * @brief K230串口错误处理函数（在错误中断回调中调用）
 * @param huart UART句柄指针，用于匹配对应的串口外设
 * @note 该函数用于清除UART错误标志（读取SR和DR寄存器），并重新启动空闲中断接收，
 *       确保串口在发生错误后能够恢复正常工作
 */
void K230_Error_Handler(UART_HandleTypeDef *huart)
{
    if(huart != K230_Buffer.huart){
        return;
    }
    (void)K230_Buffer.huart->Instance->SR;
    (void)K230_Buffer.huart->Instance->DR;
    HAL_UARTEx_ReceiveToIdle_IT(K230_Buffer.huart, K230_Buffer.Processptr, K230_RX_BUFFER_SIZE); // 启动接收中断
}