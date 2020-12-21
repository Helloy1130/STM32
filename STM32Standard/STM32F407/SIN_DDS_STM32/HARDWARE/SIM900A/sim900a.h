#ifndef __SIM900A_H__
#define __SIM900A_H__	 
#include "sys.h"
 	
  
#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//高低字节交换宏定义

void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
void sim900a_load_keyboard(u16 x,u16 y,u8 **kbtbl);
u8 sim900a_get_keynum(u16 x,u16 y);
void sim900a_sms_send_test(void);
#endif

