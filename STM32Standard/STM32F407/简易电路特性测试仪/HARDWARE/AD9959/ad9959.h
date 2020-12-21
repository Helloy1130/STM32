#ifndef _AD9959_H_
#define _AD9959_H_
#include "sys.h"
#include "stdint.h"
//AD9959�Ĵ�����ַ����
#define CSR_ADD   0x00   //CSR ͨ��ѡ��Ĵ���
#define FR1_ADD   0x01   //FR1 ���ܼĴ���1
#define FR2_ADD   0x02   //FR2 ���ܼĴ���2
#define CFR_ADD   0x03   //CFR ͨ�����ܼĴ���
#define CFTW0_ADD 0x04   //CTW0 ͨ��Ƶ��ת���ּĴ���
#define CPOW0_ADD 0x05   //CPW0 ͨ����λת���ּĴ���
#define ACR_ADD   0x06   //ACR ���ȿ��ƼĴ���
#define LSRR_ADD  0x07   //LSR ͨ������ɨ��Ĵ���
#define RDW_ADD   0x08   //RDW ͨ����������ɨ��Ĵ���
#define FDW_ADD   0x09   //FDW ͨ����������ɨ��Ĵ���
//AD9959�ܽź궨��
#define AD9959_CS		PGout(8)
#define AD9959_SCLK		PCout(6)
#define AD9959_UPDATE	PCout(7)
#define AD9959_PS0		PCout(8)
#define AD9959_PS1		PCout(9)
#define AD9959_PS2		PAout(8)
#define AD9959_PS3		PAout(15)
#define AD9959_SDIO0	PCout(10)
#define AD9959_SDIO1	PCout(11)
#define AD9959_SDIO2	PCout(12)
#define AD9959_SDIO3	PDout(2)
#define AD9959_PWR		PDout(3)
#define AD9959_Reset	PDout(6)

void delay1 (u32 length);
void IntReset(void);	  //AD9959��λ
void IO_Update(void);   //AD9959��������
void Intserve(void);		   //IO�ڳ�ʼ��
void WriteData_AD9959(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData,u8 temp);
void Init_AD9959(void);
void Write_frequence(u8 Channel,u32 Freq);
void Write_Amplitude(u8 Channel, u16 Ampli);
void Write_Phase(u8 Channel,u16 Phase);


#endif
