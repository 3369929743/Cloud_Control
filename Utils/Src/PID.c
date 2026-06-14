#include "PID.h"

void PID_Init(PID_t *PID, PID_confg_t *Config)
{
    PID->Kp = Config->Kp;
    PID->Ki = Config->Ki;
    PID->Kd = Config->Kd;
    PID->IntMax = Config->IntMax;
    if(Config->IntMin == 0) PID->IntMin = -PID->IntMax ;
    else PID->IntMin = Config->IntMin;    
    PID->OutMax = Config->OutMax;
    if(Config->OutMin == 0) PID->OutMin = -PID->OutMax;
    else PID->OutMin = Config->OutMin;
}

void PID_Set_Target(PID_t *PID, float Target)
{
    PID->Target = Target;
}

void PID_Set_Actual(PID_t *PID, float Actual)
{
    PID->Actual = Actual;
}

void PID_Update(PID_t *PID)
{
    PID->Pre_Error = PID->Cur_Error;
    PID->Cur_Error = PID->Target - PID->Actual;
    PID->ErrorInt += PID->Cur_Error;

    if (PID->ErrorInt > PID->IntMax)
    {
        PID->ErrorInt = PID->IntMax;
    }
    else if (PID->ErrorInt < PID->IntMin)
    {
        PID->ErrorInt = PID->IntMin;
    }

    PID->Output = PID->Kp * PID->Cur_Error + PID->Ki * PID->ErrorInt + PID->Kd * (PID->Cur_Error - PID->Pre_Error);
    if (PID->Output > PID->OutMax)
    {
        PID->Output = PID->OutMax;
    }
    else if (PID->Output < PID->OutMin)
    {
        PID->Output = PID->OutMin;
    }
}

float PID_Get_Output(PID_t *PID)
{
    return PID->Output;
}

float PID_Calculate(PID_t *PID, float Actual)
{
    PID->Actual = Actual;
    PID_Update(PID);
    return PID->Output;
}
