#ifndef __CLOUD_H__
#define __CLOUD_H__

#include "Emm_v5.h"

typedef struct {
    Serial_t *Serial_K230;
    
    Emm_Pos_t Emm_Yaw;
    Emm_Pos_t Emm_Pitch;
} Cloud_t;

#endif
