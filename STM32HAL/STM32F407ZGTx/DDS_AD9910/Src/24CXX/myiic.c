#include "myiic.h"
#include "delay.h"

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	MY_IIC_SDA_HIGH;	  	  
	MY_IIC_SCL_HIGH;
	delay_us(4);
 	MY_IIC_SDA_LOW;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	MY_IIC_SCL_LOW;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	MY_IIC_SCL_LOW;
	MY_IIC_SDA_LOW;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	MY_IIC_SCL_HIGH; 
	MY_IIC_SDA_HIGH;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	MY_IIC_SDA_HIGH;delay_us(1);	   
	MY_IIC_SCL_HIGH;delay_us(1);	 
	while(MY_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	MY_IIC_SCL_LOW;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	MY_IIC_SCL_LOW;
	SDA_OUT();
	MY_IIC_SDA_LOW;
	delay_us(2);
	MY_IIC_SCL_HIGH;
	delay_us(2);
	MY_IIC_SCL_LOW;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	MY_IIC_SCL_LOW;
	SDA_OUT();
	MY_IIC_SDA_HIGH;
	delay_us(2);
	MY_IIC_SCL_HIGH;
	delay_us(2);
	MY_IIC_SCL_LOW;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    MY_IIC_SCL_LOW;//����ʱ�ӿ�ʼ���ݴ���
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
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		MY_IIC_SCL_HIGH;
		delay_us(2); 
		MY_IIC_SCL_LOW;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        MY_IIC_SCL_LOW; 
        delay_us(2);
		MY_IIC_SCL_HIGH;
        receive<<=1;
        if(MY_READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}


