#ifndef _AD9910_H
#define _AD9910_H
#include "sys.h"

//����IO
//#define PDCLK PCout(6)//PDCLK
//#define PLL_LOCK PCout(7)//����
//#define SYNC_CLK PCout(9)//����
//#define TXENABLE PCout(11)//TXEN
//#define RSO PCout(12)//RAM-SWP-OVR

#define AD9910_PWR PDout(3)//0
//ԭʼIO
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


void Txcfr(void);//ad9910����cfr�����ֳ���
void AD9910_Init(void);
void Freq_convert(u32 Freq);//����Ƶƫ�֡�Ƶ���ֺͷ��ͳ��� ��λHz ���뷶Χ��1-400 000 000Hz
void Write_Amplitude(u16 Vpp);//��������ֺͷ��ͳ��� ��λmV ���뷶Χ��1-650 mV

//ɨƵ���������úͷ��ͳ���
//ɨƵ������Ƶ�ʣ�����Ƶ�ʣ�Ƶ�ʲ�������λ��Hz��������ʱ��������λ��ns��
void SweepFre(u32 SweepMinFre, u32 SweepMaxFre, u32 SweepStepFre, u32 SweepTime);

#endif
