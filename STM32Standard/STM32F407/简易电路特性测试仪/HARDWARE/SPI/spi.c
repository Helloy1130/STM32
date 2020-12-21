#include "spi.h"

//————————————————————————
//完整spi数据传输(22组16位数据)
//16位一次传输
//MSB16位:   开始标志0x1100
//16位数据*20
//16位：     结束标志0xff00
//————————————————————————
u8 end_flag = 0;	//结束标志0xff,用完置0
u16 outputData[20];//存储检测到的输出数据

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

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {} //等待发送区空

    SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {} //等待接收完一个byte

    return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据

}

//软件模拟SPI
void MySPI_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟

	//GPIOE2,3,4初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 |GPIO_Pin_3|GPIO_Pin_4; //PE2,PE3,PE4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化
    
	//GPIOE5初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PE5
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//浮空输入
    GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化


    GPIO_SetBits(GPIOE,GPIO_Pin_2);//时钟信号默认高
    GPIO_SetBits(GPIOE,GPIO_Pin_3);//数据片选信号默认为高
	GPIO_ResetBits(GPIOE,GPIO_Pin_4);//MOSI默认为低
}

//SPI读取数据,16位一次
u16 SPI_ReadData(void)
{
	u16 data = 0;
	u8 i;
	SPI_CS = 0;
    for(i=0; i<16; i++)
    {
		SPI_SCLK=1;

        SPI_SCLK=0;

        data <<= 1;// 左移
        data = data | SPI_MISO;// 接收数据
    }
	SPI_CS = 1;
	
	return data;
	
}

//返回值若为开始标志0x11,则开始读数据
u8 MySPI_ReadByte(void)
{
    u8 i,k;
    u8 start_flag = 0;

    SPI_CS = 0;
    for(i=0; i<8; i++)//开始标志0x11
    {
		SPI_SCLK=1;

        SPI_SCLK=0;

        start_flag <<= 1;// 左移
        start_flag = start_flag | SPI_MISO;// 接收数据
    }
	SPI_CS = 1;
	
    if(start_flag == 0x11)//开始接收真正的数据
    {
		for(k=0;k<20;k++)
		{
			outputData[k]=SPI_ReadData();
		}

		SPI_CS = 0;
		for(i=0; i<8; i++) //结束标志0xff
        {
			SPI_SCLK=1;

			SPI_SCLK=0;

            end_flag <<= 1;// 左移
            end_flag = end_flag | SPI_MISO;// 接收数据
        }
		SPI_CS = 1;
    }
    else start_flag = 0;
	
    return start_flag;
}

//发送数据
void MySPI_SendByte(u8 data)
{
	u8 i;
	
	SPI_CS = 0;
	
	for(i=0;i<8;i++)
	{	
		SPI_SCLK=0;				 
		if((data&0x80) == 0x80)// 处理得到一位并发送
		{
			SPI_MOSI=1;
		}			
		else
		{
			SPI_MOSI=0;
		}		
		data <<= 1;// 数据左移1位		
		SPI_SCLK=1;// 上升沿写入数据 	
	}	
	
	SPI_CS = 1;
}



