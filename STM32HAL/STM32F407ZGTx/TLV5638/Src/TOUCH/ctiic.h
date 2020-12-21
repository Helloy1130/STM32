#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H

#include "main.h"

//IO��������
#define CT_SDA_IN()  {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=0<<2*11;}	//PF11����ģʽ
#define CT_SDA_OUT() {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=1<<2*11;} 	//PF11���ģʽ

//IO��������
#define CT_IIC_SCL_HIGH     HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_Port,CT_IIC_SCL_Pin,GPIO_PIN_SET)
#define CT_IIC_SCL_LOW      HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_Port,CT_IIC_SCL_Pin,GPIO_PIN_RESET)
#define CT_IIC_SDA_HIGH     HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_Port,CT_IIC_SDA_Pin,GPIO_PIN_SET)
#define CT_IIC_SDA_LOW      HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_Port,CT_IIC_SDA_Pin,GPIO_PIN_RESET)
#define CT_READ_SDA         HAL_GPIO_ReadPin(CT_IIC_SDA_GPIO_Port,CT_IIC_SDA_Pin)
 

//IIC���в�������
void CT_IIC_Start(void);				//����IIC��ʼ�ź�
void CT_IIC_Stop(void);	  				//����IICֹͣ�ź�
void CT_IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t CT_IIC_Read_Byte(unsigned char ack);	//IIC��ȡһ���ֽ�
uint8_t CT_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void CT_IIC_Ack(void);					//IIC����ACK�ź�
void CT_IIC_NAck(void);					//IIC������ACK�ź�

#endif
