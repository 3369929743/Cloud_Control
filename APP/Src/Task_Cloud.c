#include "Task_Cloud.h"
#include "Cloud.h"
#include "BSP_Config.h"
#include "Serial.h"
#include "K230.h"

// Serial_t Serial_K230;
// Serial_t Serial_Yaw;
// Serial_t Serial_Pitch;

// Cloud_t Cloud;

// Cloud_Confg_t Cloud_Confg={
//     .Serial_Vision = &Serial_K230,
//     .Serial_Yaw = &Serial_Yaw,
//     .Serial_Pitch = &Serial_Pitch,
//     .PID_Yaw_Confg = {0.5, 0, 0 },
//     .PID_Pitch_Confg = {0.5, 0, 0 },
// };

void Task_Cloud_Init(void)
{
    // Serial_Init(&Serial_K230, 1);
    // Serial_Init(&Serial_Yaw, 2);
    // Serial_Init(&Serial_Pitch, 3);
    // K230_Init(Cloud_Get_Vision_Serial(&Cloud), K230_OFFSET_X, K230_OFFSET_Y);
    // Cloud_Init(&Cloud, &Cloud_Confg);
}

void Task_Cloud_Loop(void)
{
    // Cloud_Set_Target(&Cloud, 0, 0);
    // Cloud_Run(&Cloud, K230_GetError_x(), K230_GetError_y());
}

void Task_Cloud_Exit(void)
{
    
}
