#ifndef _KEY_H
#define _KEY_H

#include "sys.h"
//独立按键端口定义
#define KEY1 PAin(15)	
#define KEY0 PCin(5) 
#define WK_UP PAin(0)

#define KEY0_PRES	1	//KEY0按下		
#define KEY1_PRES	2	//KEY1按下
#define WK_UP_PRES	3	//WK_UP按下

void KEY_Init(void);	//按键IO初始化
//按键扫描函数 mode:0,不支持连续按;1,支持连续按;
//返回值：
//0，没有任何按键按下
//KEY0_PRES， KEY0 按下
//KEY1_PRES， KEY1 按下
//WKUP_PRES， WK_UP 按下
u8 KEY_Scan(u8 mode);

#endif
