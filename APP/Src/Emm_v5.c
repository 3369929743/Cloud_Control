#include "Emm_v5.h"

void Emm_Pos_Control(uint8_t Addr, uint8_t Dir, uint16_t Vel,uint8_t Acc, uint32_t Pul, bool raF, bool snF)
{
	uint8_t cmd[16] = {0};
	
	cmd[0] = Addr;                  // 地址
	cmd[1] = 0xFD;                  // 功能码
	cmd[2] = Dir;                   // 方向
	cmd[3] = (uint8_t)(Vel >> 8);   // 速度高8位
	cmd[4] = (uint8_t)(Vel >> 0);   // 速度低8位
	cmd[5] = Acc;                   // 加速度
	cmd[6] = (uint8_t)(Pul >> 24); // 脉冲数（bit24-bit31）
	cmd[7] = (uint8_t)(Pul >> 16); // 脉冲数（bit16-bit23）
	cmd[8] = (uint8_t)(Pul >> 8);  // 脉冲数（bit8-bit15）
	cmd[9] = (uint8_t)(Pul >> 0);  // 脉冲数（bit0-bit7）
	cmd[10] = raF;                 // 相对/绝对标志，0：相对，1：绝对
	cmd[11] = snF;                 // 多机同步运动标志，0：不同步，1：同步
	cmd[12] = 0x6B;                // 校验和
	
}		
