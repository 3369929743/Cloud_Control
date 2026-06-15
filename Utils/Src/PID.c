#include "PID.h"

void PID_Init(PID_t *PID, PID_confg_t *Config)
{
#if PID_USE_FLOAT
    PID->Kp = Config->Kp;
    PID->Ki = Config->Ki;
    PID->Kd = Config->Kd;
    PID->IntMax = Config->IntMax;
    PID->IntMin = (Config->IntMin == 0) ? -PID->IntMax : Config->IntMin;
    PID->OutMax = Config->OutMax;
    PID->OutMin = (Config->OutMin == 0) ? -PID->OutMax : Config->OutMin;
#else
    PID->Kp = (int32_t)(Config->Kp * PID_ZOOM);    
    PID->Ki = (int32_t)(Config->Ki * PID_ZOOM);
    PID->Kd = (int32_t)(Config->Kd * PID_ZOOM);
    PID->IntMax = (int32_t)Config->IntMax;
    PID->IntMin = (Config->IntMin == 0) ? -PID->IntMax : Config->IntMin;
    PID->OutMax = (int32_t)Config->OutMax;
    PID->OutMin = (Config->OutMin == 0) ? -PID->OutMax : Config->OutMin;
#endif
    PID_Clear(PID);
}

void PID_Clear(PID_t *PID)
{
    PID->ErrorInt = 0;
    PID->Pre_Error = 0;
    PID->Cur_Error = 0;
    PID->Output = 0;
    PID->Target = 0;
    PID->Actual = 0;
}

#if !PID_USE_FLOAT
static inline int32_t PID_Restore(int32_t x)
{
    int32_t abs_x = x > 0 ? x : -x;
    return (x>0) ? ( abs_x + PID_ZOOM / 2) >> MULTIPLE : - (( abs_x + PID_ZOOM / 2) >> MULTIPLE);
}
#endif

void PID_Set_Target(PID_t *PID, PID_val Target)
{
    PID->Target = Target;
}

void PID_Set_Actual(PID_t *PID, PID_val Actual)
{
    PID->Actual = Actual; 
}

static void PID_Update(PID_t *PID)
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
#if !PID_USE_FLOAT
    PID->Output = PID_Restore(PID->Output);
#endif
    if (PID->Output > PID->OutMax)
    {
        PID->Output = PID->OutMax;
    }
    else if (PID->Output < PID->OutMin)
    {
        PID->Output = PID->OutMin;
    }
}

PID_val PID_Get_Output(PID_t *PID)
{
    return PID->Output;
}

PID_val PID_Calculate(PID_t *PID, PID_val Actual)
{
    PID->Actual = Actual ;
    PID_Update(PID);
    return PID->Output;
}

void PID_Change_Param(PID_t *PID, float Kp, float Ki, float Kd)
{
#if PID_USE_FLOAT
    PID->Kp = Kp;
    PID->Ki = Ki;
    PID->Kd = Kd;
#else
    PID->Kp = (int32_t)(Kp * PID_ZOOM);    
    PID->Ki = (int32_t)(Ki * PID_ZOOM);
    PID->Kd = (int32_t)(Kd * PID_ZOOM);
#endif
    PID_Clear(PID);
}
