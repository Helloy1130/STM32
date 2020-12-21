//
// Created by Y on 2020/10/10.
//

#include "ParallelCommunication.h"
#include "FFT.h"
#include <stdio.h>

extern float fft_inputbuf[FFT_LENGTH*2];

float wave_buf[FFT_LENGTH];

static int16_t ParallelReadData(void)
{
    int16_t data = 0;

    FPGA_DATA_CS_LOW;

    for (int k = 0; k < 100; ++k);

    if(FPGA_DATA0)  data = data | 0x0001;
    if(FPGA_DATA1)  data = data | 0x0002;
    if(FPGA_DATA2)  data = data | 0x0004;
    if(FPGA_DATA3)  data = data | 0x0008;
    if(FPGA_DATA4)  data = data | 0x0010;
    if(FPGA_DATA5)  data = data | 0x0020;
    if(FPGA_DATA6)  data = data | 0x0040;
    if(FPGA_DATA7)  data = data | 0x0080;
    if(FPGA_DATA8)  data = data | 0x0100;
    if(FPGA_DATA9)  data = data | 0x0200;
    if(FPGA_DATA10) data = data | 0x0400;
    if(FPGA_DATA11) data = data | 0x0800;
    if(FPGA_DATA12) data = data | 0x1000;
    if(FPGA_DATA13) data = data | 0x2000;
    if(FPGA_DATA14) data = data | 0x4000;
    if(FPGA_DATA15) data = data | 0x8000;

    FPGA_DATA_CS_HIGH;

    for (int k = 0; k < 100; ++k);

    return data;
}

void getWaveData(void)
{
    int16_t data;
    for (int i = 0; i < FFT_LENGTH; ++i)
    {
        data = ParallelReadData();
        fft_inputbuf[2*i]=data / 32768.0 * 10;	//生成输入信号实部
        fft_inputbuf[2*i+1]=0;//虚部全部为0
        wave_buf[i]=fft_inputbuf[2*i];
//        printf("%d\r\n",data);
    }
}
