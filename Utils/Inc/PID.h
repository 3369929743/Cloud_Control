#ifndef __PID_H__
#define __PID_H__

typedef struct
{
	float Kp, Ki, Kd;

	float Pre_Error, Cur_Error, ErrorInt, IntMax, IntMin;

    float Actual, Target;

    float OutMax, OutMin;

	float Output;
} PID_t;

typedef struct {
    float Kp, Ki, Kd;

    float IntMax, IntMin;

    float OutMax, OutMin;
}PID_confg_t;

void PID_Init(PID_t *PID, PID_confg_t *Config);
void PID_Set_Target(PID_t *PID, float Target);
void PID_Set_Actual(PID_t *PID, float Actual);
void PID_Update(PID_t *PID);
float PID_Get_Output(PID_t *PID);
float PID_Calculate(PID_t *PID, float Actual);

#endif