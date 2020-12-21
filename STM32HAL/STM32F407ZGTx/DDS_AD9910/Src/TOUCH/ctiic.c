#include "ctiic.h"
#include "delay.h"

//控制I2C速度的延时
void CT_Delay(void)
{
	delay_us(2);
} 

//产生IIC起始信号
void CT_IIC_Start(void)
{
	CT_SDA_OUT();     //sda线输出
	CT_IIC_SDA_HIGH;
	CT_IIC_SCL_HIGH;
	delay_us(30);
 	CT_IIC_SDA_LOW;//START:when CLK is high,DATA change form high to low 
	CT_Delay();
	CT_IIC_SCL_LOW;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void CT_IIC_Stop(void)
{
	CT_SDA_OUT();//sda线输出
	CT_IIC_SCL_HIGH;
	delay_us(30);
	CT_IIC_SDA_LOW;//STOP:when CLK is high DATA change form low to high
	CT_Delay();
	CT_IIC_SCL_HIGH;//发送I2C总线结束信号  
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t CT_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	CT_SDA_IN();      //SDA设置为输入
	CT_IIC_SCL_HIGH;	   
	CT_IIC_SCL_HIGH;
	CT_Delay();
	while(CT_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
		CT_Delay();
	}
	CT_IIC_SCL_LOW;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void CT_IIC_Ack(void)
{
	CT_IIC_SCL_LOW;
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA_LOW;
	CT_Delay();
	CT_IIC_SCL_HIGH;
	CT_Delay();
	CT_IIC_SCL_LOW;
}
//不产生ACK应答		    
void CT_IIC_NAck(void)
{
	CT_IIC_SCL_LOW;
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SCL_HIGH;
	CT_Delay();
	CT_IIC_SCL_HIGH;
	CT_Delay();
	CT_IIC_SCL_LOW;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void CT_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	CT_SDA_OUT();
    CT_IIC_SCL_LOW;//拉低时钟开始数据传输
	CT_Delay();
	for(t=0;t<8;t++)
    {
	    if ((txd&0x80)>>7)
        {
            CT_IIC_SDA_HIGH;
        }
	    else
        {
            CT_IIC_SDA_LOW;
        }
        txd<<=1; 	      
		CT_IIC_SCL_HIGH; 
		CT_Delay();
		CT_IIC_SCL_LOW;	
		CT_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t CT_IIC_Read_Byte(unsigned char ack)
{
	uint8_t i,receive=0;
 	CT_SDA_IN();//SDA设置为输入
	delay_us(30);
	for(i=0;i<8;i++ )
	{ 
		CT_IIC_SCL_LOW; 	    	   
		CT_Delay();
		CT_IIC_SCL_HIGH;	 
		receive<<=1;
		if(CT_READ_SDA)receive++;   
	}	  				 
	if (!ack)CT_IIC_NAck();//发送nACK
	else CT_IIC_Ack(); //发送ACK   
 	return receive;
}
