#include "spi.h"
 
//������������������������������������������������
//һ��spi���ݴ���
//MSB8λ:   ��ʼ��־0x11
//32λ��    �����ź�ռ�ձȼ���ֵ
//32λ��    ˫ͨ��ͬƵ���źŵ�ʱ��������ֵ
//32λ��    �����ź�Ƶ�ʼ���ֵ
//32λ��   	��׼�ź�Ƶ�ʼ���ֵ
//8λ��     ������־0xff
//8λ��   	У����0x0a
//8λ��   	У����0x0b
//������������������������������������������������
u32 fbase_cnt = 0;  //��׼�ź�Ƶ�ʼ���ֵ
u32 fx_cnt = 0;     //�����ź�Ƶ�ʼ���ֵ 
u32 time_cnt = 0;   //˫ͨ��ͬƵ���źŵ�ʱ��������ֵ
u32 duty_cnt = 0;   //�����ź�ռ�ձȼ���ֵ
u8 end_flag = 0;	//������־0xff
u8 check_flag1 = 0;	//У����0x0a
u8 check_flag2 = 0;	//У����0x0a

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
 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ��byte  ����
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
 
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	
 		    
}

////Ӳ��SPI3
//void SPI3_Init(void)
//{	 
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	SPI_InitTypeDef  SPI_InitStructure;

//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//ʹ��SPI3ʱ��

//	
//	//GPIOA15��ʼ������
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PA15���ù������	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//�������
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��

//	//GPIOC10,11��ʼ������
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;//PC10,11���ù������	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
//	
//	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3); //PC10����Ϊ SPI3_SCK
//	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3); //PC11����Ϊ SPI3_MISO

//	//����ֻ���SPI�ڳ�ʼ��
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,ENABLE);//��λSPI1
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,DISABLE);//ֹͣ��λSPI1

//	SPI_Cmd(SPI3, DISABLE);	 //�����Ƚ���,���ܸı�MODE
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵ�1��������,���ݱ�����
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ2
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
//	SPI_Init(SPI3, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

//	SPI_Cmd(SPI3, ENABLE); //ʹ��SPI����
//	
//	//SPI3_ReadWriteByte(0xff);//��������	
// 
//}   

////SPI3 ��дһ���ֽ�
////TxData:Ҫд����ֽ�
////����ֵ:��ȡ�����ֽ�
//u8 SPI3_ReadWriteByte(u8 TxData)
//{		
//	SPI_CS = 0;//����Ƭѡ�ź�����
//	
//	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
//	SPI_I2S_SendData(SPI3, TxData); //ͨ������SPIx����һ��byte  ����	
//		
//	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte
//	
//	GPIO_SetBits(GPIOA,GPIO_Pin_15);//����Ƭѡ�ź�����
// 
//	return SPI_I2S_ReceiveData(SPI3); //����ͨ��SPIx������յ�����
//}


//���ģ��SPI
void SPI3_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	
	//GPIOA0��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	//GPIOA5,7��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;//PA15�������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	
	SPI_SCK = 1;//ʱ���ź�Ĭ�ϸ�
	SPI_CS = 1;//����Ƭѡ�ź�Ĭ��Ϊ��
} 

u8 SPI3_Read_Byte(void) 
{
	u8 i;
	u8 start_flag = 0;
	
	SPI_CS = 0;
	for(i=0;i<8;i++)
	{		
		SPI_SCK = 1;
		
		SPI_SCK = 0;
		
		start_flag <<= 1;// ����	
		start_flag = start_flag | SPI_MISO;// ��������
	}
	if(start_flag == 0x11)//��ʼ����
	{
		for(i=0;i<32;i++)//�����ź�ռ�ձȼ���ֵ
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			duty_cnt <<= 1;// ����	
			duty_cnt = duty_cnt | SPI_MISO;// ��������
		}
		for(i=0;i<32;i++)//˫ͨ��ͬƵ���źŵ�ʱ��������ֵ
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			time_cnt <<= 1;// ����	
			time_cnt = time_cnt | SPI_MISO;// ��������
		}
		for(i=0;i<32;i++)//�����ź�Ƶ�ʼ���ֵ
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			fx_cnt <<= 1;// ����	
			fx_cnt = fx_cnt | SPI_MISO;// ��������
		}
		for(i=0;i<32;i++)//��׼�ź�Ƶ�ʼ���ֵ
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			fbase_cnt <<= 1;// ����	
			fbase_cnt = fbase_cnt | SPI_MISO;// ��������
		}
		for(i=0;i<8;i++)//������־0xff
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			end_flag <<= 1;// ����	
			end_flag = end_flag | SPI_MISO;// ��������
		}
		for(i=0;i<8;i++)//У����0x0a
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			check_flag1 <<= 1;// ����	
			check_flag1 = check_flag1 | SPI_MISO;// ��������
		}
		for(i=0;i<8;i++)//У����0x0b
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			check_flag2 <<= 1;// ����	
			check_flag2 = check_flag2 | SPI_MISO;// ��������
		}
	}
	else start_flag = 0;
	SPI_CS = 1;
	return start_flag;	            								
}



