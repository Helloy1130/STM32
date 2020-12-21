#ifndef _GPIO_H
#define _GPIO_H
#include "sys.h"


#define startSignal	PEout(6)//与FPGA连接，高电平有效
#define Relay_inout	PAout(4)//继电器控制测输入输出，低电平输入，高电平输出
#define Relay_RS	PAout(5)//继电器控制采样电阻，低电平接采样电阻，高电平不接
#define Relay_RL	PAout(6)//继电器控制负载，低电平空载，高电平带载

void myGPIO_init(void);

#endif
