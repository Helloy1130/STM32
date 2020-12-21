#include "ad9910.h"
#include "delay.h"

u8 cfr1[] = {0x00, 0x40, 0x00, 0x00};	//CFR1控制字
u8 cfr2[] = {0x01, 0x00, 0x00, 0x00}; 		//CFR2控制字
u8 cfr3[] = {0x05, 0x0f, 0x41, 0x32}; //CFR3控制字40M输入  25倍频  VC0=101   ICP=001;
u8 profile0[] = {0x3f, 0xff, 0x00, 0x00, 0x25, 0x09, 0x7b, 0x42};	//Profile0控制字 0x25,0x09,0x7b,0x42
//---------------01振幅控制    23相位控制      4567频率调谐字
u8 drgparameter[20]= {0x00}; //DRG参数
//----------------高字节								低字节

//------------------------------
//AD9910 IO初始化
void AD9110_IOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE); //使能PA PC PD端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 |GPIO_Pin_15; //PA8,PA15
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12; //PC6,PC7,PC8,PC9,PC10,PC11,PC12
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 |GPIO_Pin_3; //PD2 PD3
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
}

//------------------------------
//发送8位数据程序(高位在前，上升沿发送)
void txd_8bit(u8 txdat)
{
    u8 i, sbt;
    sbt = 0x80;
    AD9910_SCLK = 0;
    for (i = 0; i < 8; i++)
    {
        if ((txdat & sbt) == 0)
            AD9910_SDO = 0;
        else
            AD9910_SDO = 1;
        AD9910_SCLK = 1;
        sbt = sbt >> 1;
        AD9910_SCLK = 0;
    }
}

//------------------------------
//ad9910发送cfrx控制字程序
void Txcfr(void)
{
	u8 m,k;

	AD9910_CS=0;
    txd_8bit(0x00);    //发送CFR1控制字地址
    for (m=0; m<4; m++)
    {
        txd_8bit(cfr1[m]);
    }
    AD9910_CS=1;
    for (k=0; k<10; k++);
	
	AD9910_CS=0;
	txd_8bit(0x01);    //发送CFR2控制字地址
	for (m=0;m<4;m++)
	{
		txd_8bit(cfr2[m]);
	}
	AD9910_CS=1;
	for (k=0;k<10;k++);

    AD9910_CS=0;
    txd_8bit(0x02);    //发送CFR3控制字地址
    for (m=0; m<4; m++)
    {
        txd_8bit(cfr3[m]);
    }
    AD9910_CS=1;
    for (k=0; k<10; k++);

	UP_DAT=1;
	for(k=0;k<10;k++);
	UP_DAT=0;
	delay_ms(1);
} 

//------------------------------
//AD9910初始化
void AD9910_Init(void)
{
    AD9110_IOInit();//IO初始化
    AD9910_PWR = 0;//软件拉低
    PROFILE2=PROFILE1=PROFILE0=0;
    DRCTL=0;
    DRHOLD=0;
    MASTRST=1;
    delay_ms(5);
    MASTRST=0;

	Txcfr();//发送cfrx控制字程序
}

//------------------------------
//AD9910发送profile0控制字程序
void Txprofile(void)
{
    u8 m, k;

    AD9910_CS=0;
    txd_8bit(0x0e);    //发送profile0控制字地址
    for (m=0; m<8; m++)
    {
        txd_8bit(profile0[m]);
    }
    AD9910_CS=1;
    for(k=0; k<10; k++);

    UP_DAT=1;
    for(k=0; k<10; k++);
    UP_DAT=0;
    delay_ms(1);
}

//------------------------------
//计算频偏字、频率字和发送程序
//单位Hz 输入范围：1-400 000 000Hz
void Freq_convert(u32 Freq)
{
    u32 FTW;
    FTW=(u32)Freq*4.294967296;	   //将输入频率因子分为四个字节  4.294967296=(2^32)/1000_000_000	1G是内部时钟
    profile0[7]=(u8)FTW;
    profile0[6]=(u8)(FTW>>8);
    profile0[5]=(u8)(FTW>>16);
    profile0[4]=(u8)(FTW>>24);
    Txprofile();
}

//------------------------------
//计算幅度字和发送程序
//单位mV 输入范围：1-1000 mV
void Write_Amplitude(u16 Vpp)
{
    u32 ASF;
    ASF = (u32)Vpp*16.384;	   //将输入幅度因子分为两个字节  16.384=(2^14)/1000  满量程1000mV
    if(ASF > 0x3fff)
        ASF = 0x3fff;
    ASF &= 0x3fff;
    profile0[1]=(u8)ASF;
    profile0[0]=(u8)(ASF>>8);
    Txprofile();
}

//------------------------------
//扫频
//------------------------------
//ad9910发送DRG参数程序
void Txdrg(void)
{
    u8 m,k;

    AD9910_CS=0;
    txd_8bit(0x0b);    //发送数字斜坡限制地址0x0b
    for (m=0; m<8; m++)
        txd_8bit(drgparameter[m]);
    AD9910_CS=1;
    for(k=0; k<10; k++);

    AD9910_CS=0;
    txd_8bit(0x0c);    //发送数字斜坡步长地址0x0c
    for (m=8; m<16; m++)
        txd_8bit(drgparameter[m]);
    AD9910_CS=1;
    for(k=0; k<10; k++);

    AD9910_CS=0;
    txd_8bit(0x0d);    //发送数字斜坡速率地址0x0d
    for (m=16; m<20; m++)
        txd_8bit(drgparameter[m]);
    AD9910_CS=1;
    for(k=0; k<10; k++);

    UP_DAT=1;
    for(k=0; k<10; k++);
    UP_DAT=0;
    delay_ms(1);
}

//------------------------------
//扫频波参数设置和发送程序
//扫频波下限频率，上限频率，频率步进（单位：Hz），步进时间间隔（单位：ns）
void SweepFre(u32 SweepMinFre, u32 SweepMaxFre, u32 SweepStepFre, u32 SweepTime)
{
    u32 minFTW, maxFTW, stepFTW_P, stepFTW_N, time_P, time_N;

    minFTW = (u32)SweepMinFre*4.294967296;

    if(SweepMaxFre > 400000000)
        SweepMaxFre = 400000000;
    maxFTW = (u32)SweepMaxFre*4.294967296;

    if(SweepStepFre > 400000000)
        SweepStepFre = 400000000;
    stepFTW_P = (u32)SweepStepFre*4.294967296;//递增

    stepFTW_N = stepFTW_P;//递减

    time_P = (u32)SweepTime/4; //时间间隔=SweepTime*10^(-9) * clk(内部) /4 	内部为1GHz
    if(time_P > 0xffff)
        time_P = 0xffff;

    time_N = time_P;

    //扫频上下限
    drgparameter[7]=(u8)minFTW;
    drgparameter[6]=(u8)(minFTW>>8);
    drgparameter[5]=(u8)(minFTW>>16);
    drgparameter[4]=(u8)(minFTW>>24);

    drgparameter[3]=(u8)maxFTW;
    drgparameter[2]=(u8)(maxFTW>>8);
    drgparameter[1]=(u8)(maxFTW>>16);
    drgparameter[0]=(u8)(maxFTW>>24);

    //频率步进（单位：Hz）
    drgparameter[15]=(u8)stepFTW_P;//递增
    drgparameter[14]=(u8)(stepFTW_P>>8);
    drgparameter[13]=(u8)(stepFTW_P>>16);
    drgparameter[12]=(u8)(stepFTW_P>>24);

    drgparameter[11]=(u8)stepFTW_N;//递减
    drgparameter[10]=(u8)(stepFTW_N>>8);
    drgparameter[9]=(u8)(stepFTW_N>>16);
    drgparameter[8]=(u8)(stepFTW_N>>24);

    //步进时间间隔（单位：us）
    drgparameter[19]=(u8)time_P;//正斜率
    drgparameter[18]=(u8)(time_P>>8);

    drgparameter[17]=(u8)time_N;//负斜率
    drgparameter[16]=(u8)(time_N>>8);

    //发送DRG参数
    Txdrg();
}

