#ifndef _AD9910_H
#define _AD9910_H
#include "sys.h"

//新增IO
//#define PDCLK PCout(6)//PDCLK
//#define PLL_LOCK PCout(7)//悬空
//#define SYNC_CLK PCout(9)//悬空
//#define TXENABLE PCout(11)//TXEN
//#define RSO PCout(12)//RAM-SWP-OVR

#define AD9910_PWR PDout(3)//0
//原始IO
#define AD9910_SCLK PCout(6)
#define AD9910_CS   PCout(7)
#define AD9910_SDO  PCout(8)//SDIO

#define MASTRST    	PCout(9)
#define UP_DAT      PAout(8)

#define PROFILE0    PAout(15)
#define PROFILE1    PCout(10)
#define PROFILE2    PCout(11)

#define DRHOLD      PCout(12)
#define DRCTL       PDout(2)
//#define OSK         PDout(3)
//#define DROVER      PDout(3)


void Txcfr(void);//ad9910发送cfr控制字程序
void AD9910_Init(void);
void Freq_convert(u32 Freq);//计算频偏字、频率字和发送程序 单位Hz 输入范围：1-400 000 000Hz
void Write_Amplitude(u16 Vpp);//计算幅度字和发送程序 单位mV 输入范围：1-650 mV

//扫频波参数设置和发送程序
//扫频波下限频率，上限频率，频率步进（单位：Hz），步进时间间隔（单位：ns）
void SweepFre(u32 SweepMinFre, u32 SweepMaxFre, u32 SweepStepFre, u32 SweepTime);

#endif
