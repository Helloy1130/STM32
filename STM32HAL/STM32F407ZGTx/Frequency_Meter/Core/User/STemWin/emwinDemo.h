#ifndef _EMWINDEMO_H
#define _EMWINDEMO_H

#include "main.h"

#define GATE_HIGH       HAL_GPIO_WritePin(GATE_GPIO_Port,GATE_Pin,GPIO_PIN_SET)
#define GATE_LOW        HAL_GPIO_WritePin(GATE_GPIO_Port,GATE_Pin,GPIO_PIN_RESET)

//用户自定义消息
#define WM_SHOW_MSG (WM_USER+1)

void emwinDemo(void);

#endif
