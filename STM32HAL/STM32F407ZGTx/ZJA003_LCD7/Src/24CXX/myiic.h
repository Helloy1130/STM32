#ifndef _MYIIC_H
#define _MYIIC_H

#include "main.h"

//IO方向设置
#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9输入模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9输出模式

//IO操作
#define MY_IIC_SCL_HIGH     HAL_GPIO_WritePin(MY_IIC_SCL_GPIO_Port,MY_IIC_SCL_Pin,GPIO_PIN_SET)
#define MY_IIC_SCL_LOW      HAL_GPIO_WritePin(MY_IIC_SCL_GPIO_Port,MY_IIC_SCL_Pin,GPIO_PIN_RESET)
#define MY_IIC_SDA_HIGH     HAL_GPIO_WritePin(MY_IIC_SDA_GPIO_Port,MY_IIC_SDA_Pin,GPIO_PIN_SET)
#define MY_IIC_SDA_LOW      HAL_GPIO_WritePin(MY_IIC_SDA_GPIO_Port,MY_IIC_SDA_Pin,GPIO_PIN_RESET)
#define MY_READ_SDA         HAL_GPIO_ReadPin(MY_IIC_SDA_GPIO_Port,MY_IIC_SDA_Pin)

//IIC所有操作函数
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);

#endif

