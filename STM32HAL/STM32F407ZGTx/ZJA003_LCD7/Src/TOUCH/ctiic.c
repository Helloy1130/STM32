#include "ctiic.h"
#include "delay.h"

//����I2C�ٶȵ���ʱ
void CT_Delay(void)
{
	delay_us(2);
} 

//����IIC��ʼ�ź�
void CT_IIC_Start(void)
{
	CT_SDA_OUT();     //sda�����
	CT_IIC_SDA_HIGH;
	CT_IIC_SCL_HIGH;
	delay_us(30);
 	CT_IIC_SDA_LOW;//START:when CLK is high,DATA change form high to low 
	CT_Delay();
	CT_IIC_SCL_LOW;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void CT_IIC_Stop(void)
{
	CT_SDA_OUT();//sda�����
	CT_IIC_SCL_HIGH;
	delay_us(30);
	CT_IIC_SDA_LOW;//STOP:when CLK is high DATA change form low to high
	CT_Delay();
	CT_IIC_SCL_HIGH;//����I2C���߽����ź�  
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t CT_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	CT_SDA_IN();      //SDA����Ϊ����
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
	CT_IIC_SCL_LOW;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void CT_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	CT_SDA_OUT();
    CT_IIC_SCL_LOW;//����ʱ�ӿ�ʼ���ݴ���
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
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t CT_IIC_Read_Byte(unsigned char ack)
{
	uint8_t i,receive=0;
 	CT_SDA_IN();//SDA����Ϊ����
	delay_us(30);
	for(i=0;i<8;i++ )
	{ 
		CT_IIC_SCL_LOW; 	    	   
		CT_Delay();
		CT_IIC_SCL_HIGH;	 
		receive<<=1;
		if(CT_READ_SDA)receive++;   
	}	  				 
	if (!ack)CT_IIC_NAck();//����nACK
	else CT_IIC_Ack(); //����ACK   
 	return receive;
}
