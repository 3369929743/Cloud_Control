#ifndef __CLOUD_H__
#define __CLOUD_H__

#include "Emm_v5.h"
#include "PID.h"

typedef struct {
    Serial_t *Serial_Vision;
    
    Emm_Pos_t Emm_Yaw;
    Emm_Pos_t Emm_Pitch;

    PID_t PID_Yaw;
    PID_t PID_Pitch;
} Cloud_t;

typedef struct {
    Serial_t *Serial_Vision;
    Serial_t *Serial_Yaw;
    Serial_t *Serial_Pitch;

    PID_Confg_t PID_Yaw_Confg;
    PID_Confg_t PID_Pitch_Confg;
} Cloud_Confg_t;

void Cloud_Init(Cloud_t *Cloud, Cloud_Confg_t *Cloud_Confg);
void Cloud_Set_Target(Cloud_t *Cloud, PID_val Yaw, PID_val Pitch);
void Cloud_Run(Cloud_t *Cloud, PID_val Yaw, PID_val Pitch);
Serial_t * Cloud_Get_Vision_Serial(Cloud_t *Cloud);

#endif
