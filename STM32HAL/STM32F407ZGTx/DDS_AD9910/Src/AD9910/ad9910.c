//
// Created by Y on 2020/10/3.
//

#include "ad9910.h"

const uint8_t cfr1[]={0x00,0x40,0x00,0x00};       //cfr1控制字
const uint8_t cfr2[]={0x01,0x00,0x00,0x00};       //cfr2控制字
const uint8_t cfr3[]={0x05,0x0F,0x41,0x32};       //cfr3控制字  40M输入  25倍频  VC0=101   ICP=001;
uint8_t profile11[]={0xf0,0xff,0x00,0x00,0x25,0x09,0x7b,0x42};       //profile1控制字 01振幅控制 23相位控制 4567频率调谐字

//======================发送8位数据程序===================================
void txd_8bit(uint8_t txdat)
{
    uint8_t i,sbt;
    sbt=0x80;
    AD9910_SCLK_LOW;
    for (i=0;i<8;i++)
    {
        if ((txdat & sbt)==0)
        {
            AD9910_SDIO_LOW;
        }
        else
        {
            AD9910_SDIO_HIGH;
        }
        AD9910_SCLK_HIGH;
        sbt=sbt>>1;
        AD9910_SCLK_LOW;
    }
}

//======================ad9910初始化程序================================
void ad9910_init(void)
{
    uint8_t k,m;

    AD9910_MASREST_HIGH;
    HAL_Delay(5);
    AD9910_MASREST_LOW;

    AD9910_CS_LOW;
    txd_8bit(0x00);    //发送CFR1控制字地址
    for (m=0;m<4;m++)
    {
        txd_8bit(cfr1[m]);
    }
    AD9910_CS_HIGH;
    for (k=0;k<10;k++);

    AD9910_CS_LOW;
    txd_8bit(0x01);    //发送CFR2控制字地址
    for (m=0;m<4;m++)
    {
        txd_8bit(cfr2[m]);
    }
    AD9910_CS_HIGH;
    for (k=0;k<10;k++);

    AD9910_CS_LOW;
    txd_8bit(0x02);    //发送CFR3控制字地址
    for (m=0;m<4;m++)
    {
        txd_8bit(cfr3[m]);
    }
    AD9910_CS_HIGH;
    for (k=0;k<10;k++);

    AD9910_UPDAT_HIGH;
    for(k=0;k<10;k++);
    AD9910_UPDAT_LOW;
    HAL_Delay(1);
}

//======================ad9910发送频率值程序===================================
void Txfrc(void)
{
    uint8_t m,k;

    AD9910_CS_LOW;
    txd_8bit(0x0e);    //发送profile1控制字地址
    for (m=0;m<8;m++)
    {
        txd_8bit(profile11[m]);
    }
    AD9910_CS_HIGH;
    for(k=0;k<10;k++);

    AD9910_UPDAT_HIGH;
    for(k=0;k<10;k++);
    AD9910_UPDAT_LOW;
    HAL_Delay(1);
}

//===================计算频偏字、频率字和发送程序======================
void Freq_Amp_set(uint32_t Freq,float Amp)
{
    uint32_t Temp_freq;
    uint16_t Temp_amp;
    Temp_freq=(uint32_t)Freq*4.294967296;	   //将输入频率因子分为四个字节  4.294967296=(2^32)/1000000000
    Temp_amp=Amp*65535;

    profile11[7]=(uint8_t)Temp_freq;
    profile11[6]=(uint8_t)(Temp_freq>>8);
    profile11[5]=(uint8_t)(Temp_freq>>16);
    profile11[4]=(uint8_t)(Temp_freq>>24);
    profile11[1]=(uint8_t)Temp_amp;
    profile11[0]=(uint8_t)(Temp_amp>>8);
    Txfrc();
}
