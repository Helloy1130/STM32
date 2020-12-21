#include "myiic.h"

//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	MY_IIC_SDA_HIGH;	  	  
	MY_IIC_SCL_HIGH;
	HAL_Delay(1);
 	MY_IIC_SDA_LOW;//START:when CLK is high,DATA change form high to low
    HAL_Delay(1);
	MY_IIC_SCL_LOW;//钳住I2C总线，准备发送或接收数据
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	MY_IIC_SCL_LOW;
	MY_IIC_SDA_LOW;//STOP:when CLK is high DATA change form low to high
 	HAL_Delay(1);
 	
	MY_IIC_SCL_HIGH; 
	MY_IIC_SDA_HIGH;//发送I2C总线结束信号
	HAL_Delay(1);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	MY_IIC_SDA_HIGH;
	HAL_Delay(1);	   
	MY_IIC_SCL_HIGH;
	HAL_Delay(1);	 
	while(MY_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	MY_IIC_SCL_LOW;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	MY_IIC_SCL_LOW;
	SDA_OUT();
	MY_IIC_SDA_LOW;
	HAL_Delay(1);
	MY_IIC_SCL_HIGH;
	HAL_Delay(1);
	MY_IIC_SCL_LOW;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	MY_IIC_SCL_LOW;
	SDA_OUT();
	MY_IIC_SDA_HIGH;
	HAL_Delay(1);
	MY_IIC_SCL_HIGH;
	HAL_Delay(1);
	MY_IIC_SCL_LOW;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT();
    MY_IIC_SCL_LOW;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {
        if ((txd&0x80)>>7)
        {
            MY_IIC_SDA_HIGH;
        }
        else
        {
            MY_IIC_SDA_LOW;
        }
        txd<<=1;
		HAL_Delay(1);   //对TEA5767这三个延时都是必须的
		MY_IIC_SCL_HIGH;
		HAL_Delay(1);
		MY_IIC_SCL_LOW;
		HAL_Delay(1);
    }
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        MY_IIC_SCL_LOW; 
        HAL_Delay(1);
		MY_IIC_SCL_HIGH;
        receive<<=1;
        if(MY_READ_SDA)receive++;   
		HAL_Delay(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}


