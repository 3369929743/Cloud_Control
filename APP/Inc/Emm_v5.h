#ifndef __EMM_V5_H__
#define __EMM_V5_H__

#include "main.h"
#include "Serial.h"
#include "stdbool.h"

void Emm_Pos_Control(uint8_t Addr, uint8_t Dir, uint16_t Vel,uint8_t Acc, uint32_t Pul, bool raF, bool snF);


#endif
