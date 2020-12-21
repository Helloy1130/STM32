#include "spi.h"
 
//————————————————————————
//一次spi数据传输
//MSB8位:   开始标志0x11
//32位：    待测信号占空比计数值
//32位：    双通道同频率信号的时间间隔计数值
//32位：    待测信号频率计数值
//32位：   	标准信号频率计数值
//8位：     结束标志0xff
//8位：   	校验码0x0a
//8位：   	校验码0x0b
//————————————————————————
u32 fbase_cnt = 0;  //标准信号频率计数值
u32 fx_cnt = 0;     //待测信号频率计数值 
u32 time_cnt = 0;   //双通道同频率信号的时间间隔计数值
u32 duty_cnt = 0;   //待测信号占空比计数值
u8 end_flag = 0;	//结束标志0xff
u8 check_flag1 = 0;	//校验码0x0a
u8 check_flag2 = 0;	//校验码0x0a

//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI1的初始化
void SPI1_Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI1时钟
 
  //GPIOB3,B4,B5初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;//PB3~5复用功能输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1); //PB3复用为 SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); //PB4复用为 SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1); //PB5复用为 SPI1

 
	//这里只针对SPI口初始化
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设

	SPI1_ReadWriteByte(0xff);//启动传输		 
}   
//SPI1速度设置函数
//SpeedSet:0~7
//SPI速度=fAPB2/2^(SpeedSet+1)
//fAPB2时钟一般为84Mhz
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1&=0XFFC7;
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE);  
} 
//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
 		    
}

////硬件SPI3
//void SPI3_Init(void)
//{	 
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	SPI_InitTypeDef  SPI_InitStructure;

//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//使能SPI3时钟

//	
//	//GPIOA15初始化设置
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PA15复用功能输出	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化

//	//GPIOC10,11初始化设置
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;//PC10,11复用功能输出	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
//	
//	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3); //PC10复用为 SPI3_SCK
//	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3); //PC11复用为 SPI3_MISO

//	//这里只针对SPI口初始化
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,ENABLE);//复位SPI1
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,DISABLE);//停止复位SPI1

//	SPI_Cmd(SPI3, DISABLE);	 //必须先禁用,才能改变MODE
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第1个跳变沿,数据被采样
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//定义波特率预分频的值:波特率预分频值为2
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
//	SPI_Init(SPI3, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

//	SPI_Cmd(SPI3, ENABLE); //使能SPI外设
//	
//	//SPI3_ReadWriteByte(0xff);//启动传输	
// 
//}   

////SPI3 读写一个字节
////TxData:要写入的字节
////返回值:读取到的字节
//u8 SPI3_ReadWriteByte(u8 TxData)
//{		
//	SPI_CS = 0;//数据片选信号拉低
//	
//	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
//	SPI_I2S_SendData(SPI3, TxData); //通过外设SPIx发送一个byte  数据	
//		
//	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte
//	
//	GPIO_SetBits(GPIOA,GPIO_Pin_15);//数据片选信号拉高
// 
//	return SPI_I2S_ReceiveData(SPI3); //返回通过SPIx最近接收的数据
//}


//软件模拟SPI
void SPI3_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	
	//GPIOA0初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	//GPIOA5,7初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;//PA15推挽输出	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	
	SPI_SCK = 1;//时钟信号默认高
	SPI_CS = 1;//数据片选信号默认为高
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
		
		start_flag <<= 1;// 左移	
		start_flag = start_flag | SPI_MISO;// 接收数据
	}
	if(start_flag == 0x11)//开始接收
	{
		for(i=0;i<32;i++)//待测信号占空比计数值
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			duty_cnt <<= 1;// 左移	
			duty_cnt = duty_cnt | SPI_MISO;// 接收数据
		}
		for(i=0;i<32;i++)//双通道同频率信号的时间间隔计数值
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			time_cnt <<= 1;// 左移	
			time_cnt = time_cnt | SPI_MISO;// 接收数据
		}
		for(i=0;i<32;i++)//待测信号频率计数值
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			fx_cnt <<= 1;// 左移	
			fx_cnt = fx_cnt | SPI_MISO;// 接收数据
		}
		for(i=0;i<32;i++)//标准信号频率计数值
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			fbase_cnt <<= 1;// 左移	
			fbase_cnt = fbase_cnt | SPI_MISO;// 接收数据
		}
		for(i=0;i<8;i++)//结束标志0xff
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			end_flag <<= 1;// 左移	
			end_flag = end_flag | SPI_MISO;// 接收数据
		}
		for(i=0;i<8;i++)//校验码0x0a
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			check_flag1 <<= 1;// 左移	
			check_flag1 = check_flag1 | SPI_MISO;// 接收数据
		}
		for(i=0;i<8;i++)//校验码0x0b
		{		
			SPI_SCK = 1;
			
			SPI_SCK = 0;
			
			check_flag2 <<= 1;// 左移	
			check_flag2 = check_flag2 | SPI_MISO;// 接收数据
		}
	}
	else start_flag = 0;
	SPI_CS = 1;
	return start_flag;	            								
}



