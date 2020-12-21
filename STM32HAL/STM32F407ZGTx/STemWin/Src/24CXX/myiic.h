#ifndef _MYIIC_H
#define _MYIIC_H

#include "main.h"

//IO��������
#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9���ģʽ

//IO����
#define MY_IIC_SCL_HIGH     HAL_GPIO_WritePin(MY_IIC_SCL_GPIO_Port,MY_IIC_SCL_Pin,GPIO_PIN_SET)
#define MY_IIC_SCL_LOW      HAL_GPIO_WritePin(MY_IIC_SCL_GPIO_Port,MY_IIC_SCL_Pin,GPIO_PIN_RESET)
#define MY_IIC_SDA_HIGH     HAL_GPIO_WritePin(MY_IIC_SDA_GPIO_Port,MY_IIC_SDA_Pin,GPIO_PIN_SET)
#define MY_IIC_SDA_LOW      HAL_GPIO_WritePin(MY_IIC_SDA_GPIO_Port,MY_IIC_SDA_Pin,GPIO_PIN_RESET)
#define MY_READ_SDA         HAL_GPIO_ReadPin(MY_IIC_SDA_GPIO_Port,MY_IIC_SDA_Pin)

//IIC���в�������
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);

#endif

