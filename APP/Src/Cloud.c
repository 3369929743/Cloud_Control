#include "Cloud.h"
#include "K230.h"

void Cloud_Init(Cloud_t *Cloud, Serial_t *Serial_K230, Serial_t *Serial_Yaw, Serial_t *Serial_Pitch)
{
    Cloud->Serial_K230 = Serial_K230;
    Cloud->Emm_Yaw.Serial = Serial_Yaw;
    Cloud->Emm_Pitch.Serial = Serial_Pitch;
    Emm_Init(&Cloud->Emm_Yaw, Serial_Yaw, 0x01);
    Emm_Init(&Cloud->Emm_Pitch, Serial_Pitch, 0x01);
}
