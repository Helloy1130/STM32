#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H

#include "main.h"

//IO方向设置
#define CT_SDA_IN()  {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=0<<2*11;}	//PF11输入模式
#define CT_SDA_OUT() {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=1<<2*11;} 	//PF11输出模式

//IO操作函数
#define CT_IIC_SCL_HIGH     HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_Port,CT_IIC_SCL_Pin,GPIO_PIN_SET)
#define CT_IIC_SCL_LOW      HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_Port,CT_IIC_SCL_Pin,GPIO_PIN_RESET)
#define CT_IIC_SDA_HIGH     HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_Port,CT_IIC_SDA_Pin,GPIO_PIN_SET)
#define CT_IIC_SDA_LOW      HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_Port,CT_IIC_SDA_Pin,GPIO_PIN_RESET)
#define CT_READ_SDA         HAL_GPIO_ReadPin(CT_IIC_SDA_GPIO_Port,CT_IIC_SDA_Pin)
 

//IIC所有操作函数
void CT_IIC_Start(void);				//发送IIC开始信号
void CT_IIC_Stop(void);	  				//发送IIC停止信号
void CT_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t CT_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
uint8_t CT_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void CT_IIC_Ack(void);					//IIC发送ACK信号
void CT_IIC_NAck(void);					//IIC不发送ACK信号

#endif
