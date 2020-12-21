#include "spi.h"

//������������������������������������������������
//����spi���ݴ���(22��16λ����)
//16λһ�δ���
//MSB16λ:   ��ʼ��־0x1100
//16λ����*20
//16λ��     ������־0xff00
//������������������������������������������������
u8 end_flag = 0;	//������־0xff,������0
u16 outputData[20];//�洢��⵽���������

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ
//SPI�ڳ�ʼ��
//�������Ƕ�SPI1�ĳ�ʼ��
void SPI1_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//ʹ��SPI1ʱ��

    //GPIOB3,B4,B5��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;//PB3~5���ù������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1); //PB3����Ϊ SPI1
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); //PB4����Ϊ SPI1
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1); //PB5����Ϊ SPI1


    //����ֻ���SPI�ڳ�ʼ��
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//��λSPI1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//ֹͣ��λSPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
    SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����

    SPI1_ReadWriteByte(0xff);//��������
}
//SPI1�ٶ����ú���
//SpeedSet:0~7
//SPI�ٶ�=fAPB2/2^(SpeedSet+1)
//fAPB2ʱ��һ��Ϊ84Mhz
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    SPI1->CR1&=0XFFC7;
    SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ�
    SPI_Cmd(SPI1,ENABLE);
}
//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {} //�ȴ���������

    SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ��byte  ����

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {} //�ȴ�������һ��byte

    return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����

}

//���ģ��SPI
void MySPI_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOEʱ��

	//GPIOE2,3,4��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 |GPIO_Pin_3|GPIO_Pin_4; //PE2,PE3,PE4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
    
	//GPIOE5��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PE5
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
    GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��


    GPIO_SetBits(GPIOE,GPIO_Pin_2);//ʱ���ź�Ĭ�ϸ�
    GPIO_SetBits(GPIOE,GPIO_Pin_3);//����Ƭѡ�ź�Ĭ��Ϊ��
	GPIO_ResetBits(GPIOE,GPIO_Pin_4);//MOSIĬ��Ϊ��
}

//SPI��ȡ����,16λһ��
u16 SPI_ReadData(void)
{
	u16 data = 0;
	u8 i;
	SPI_CS = 0;
    for(i=0; i<16; i++)
    {
		SPI_SCLK=1;

        SPI_SCLK=0;

        data <<= 1;// ����
        data = data | SPI_MISO;// ��������
    }
	SPI_CS = 1;
	
	return data;
	
}

//����ֵ��Ϊ��ʼ��־0x11,��ʼ������
u8 MySPI_ReadByte(void)
{
    u8 i,k;
    u8 start_flag = 0;

    SPI_CS = 0;
    for(i=0; i<8; i++)//��ʼ��־0x11
    {
		SPI_SCLK=1;

        SPI_SCLK=0;

        start_flag <<= 1;// ����
        start_flag = start_flag | SPI_MISO;// ��������
    }
	SPI_CS = 1;
	
    if(start_flag == 0x11)//��ʼ��������������
    {
		for(k=0;k<20;k++)
		{
			outputData[k]=SPI_ReadData();
		}

		SPI_CS = 0;
		for(i=0; i<8; i++) //������־0xff
        {
			SPI_SCLK=1;

			SPI_SCLK=0;

            end_flag <<= 1;// ����
            end_flag = end_flag | SPI_MISO;// ��������
        }
		SPI_CS = 1;
    }
    else start_flag = 0;
	
    return start_flag;
}

//��������
void MySPI_SendByte(u8 data)
{
	u8 i;
	
	SPI_CS = 0;
	
	for(i=0;i<8;i++)
	{	
		SPI_SCLK=0;				 
		if((data&0x80) == 0x80)// ����õ�һλ������
		{
			SPI_MOSI=1;
		}			
		else
		{
			SPI_MOSI=0;
		}		
		data <<= 1;// ��������1λ		
		SPI_SCLK=1;// ������д������ 	
	}	
	
	SPI_CS = 1;
}



