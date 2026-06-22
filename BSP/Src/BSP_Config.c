#include "BSP_Config.h"
#include "usart.h"
#include "Serial.h"

static UART_HandleTypeDef* const Serial_Map[] = {
    NULL,
    &huart1,
    &huart2,
    &huart3,
};

#define SERIAL_MAX_NUM ((sizeof(Serial_Map) / sizeof(Serial_Map[0])) - 1)

void BSP_Serial_Init(Serial_t *Serial, uint8_t Serial_Num)
{
    if(Serial_Num > SERIAL_MAX_NUM || Serial == NULL)
    {
        return;
    }
    Serial_Init(Serial, Serial_Map[Serial_Num]);
}
