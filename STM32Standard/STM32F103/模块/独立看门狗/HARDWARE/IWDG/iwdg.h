#ifndef _IWDG_H
#define _IWDG_H

#include "sys.h"

void IWDG_Init(u8 prer,u16 rlr);
//Tout=((4× 2^prer) × rlr) /40
//其中 Tout 为看门狗溢出时间（单位为 ms）； prer 为看门狗时钟预分频值（IWDG_PR 值），
//范围为 0~7； rlr 为看门狗的重装载值（IWDG_RLR 的值）；
void IWDG_Feed(void);//喂独立看门狗

#endif
