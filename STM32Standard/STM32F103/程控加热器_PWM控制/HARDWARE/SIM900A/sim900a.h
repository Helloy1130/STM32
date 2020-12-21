#ifndef __SIM900A_H__
#define __SIM900A_H__	 
#include "sys.h"
#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//高低字节交换宏定义

void sim900a_test(void);//sim900a主测试程序 
void sim_send_sms(u8*phonenumber,u8*msg);
void sim_at_response(u8 mode);	
u8* sim900a_check_cmd(u8 *str);
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 sim900a_chr2hex(u8 chr);
u8 sim900a_hex2chr(u8 hex);
void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode);
void sim900a_load_keyboard(u16 x,u16 y,u8 **kbtbl);
void sim900a_key_staset(u16 x,u16 y,u8 keyx,u8 sta);
u8 sim900a_get_keynum(u16 x,u16 y);
u8 sim900a_call_test(void);			//拨号测试
void sim900a_sms_read_test(void);	//读短信测试
void sim900a_sms_send_test(void);	//发短信测试 
void sim900a_sms_ui(u16 x,u16 y);	//短信测试UI界面函数
u8 sim900a_sms_test(void);			//短信测试
void sim900a_mtest_ui(u16 x,u16 y);	//SIM900A主测试UI
u8 sim900a_gsminfo_show(u16 x,u16 y);//显示GSM模块信息
void sim900a_test(void);			//SIM900A主测试函数
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);//显示汉字
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode);
#endif





