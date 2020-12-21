//
// Created by Y on 2020/10/10.
//

#include "FFT.h"
#include "arm_math.h"
#include "tim.h"
#include <stdio.h>

float THD;
float time;

float fft_inputbuf[FFT_LENGTH*2];	//FFT输入数组
float fft_outputbuf[FFT_LENGTH];	//FFT输出数组

uint8_t timeout;
arm_cfft_radix2_instance_f32 scfft;

void FFT_Init(void)
{
    arm_cfft_radix2_init_f32(&scfft,FFT_LENGTH,0,1);//初始化scfft结构体，设定FFT相关参数
}

void FFT_Process(void)
{
//    for(int i=0;i<FFT_LENGTH;i++)//生成信号序列
//    {
////        fft_inputbuf[2*i]=arm_sin_f32(2*PI*i/FFT_LENGTH);	//生成输入信号实部
//        fft_inputbuf[2*i]=Wave_Data[i];	//生成输入信号实部
//        fft_inputbuf[2*i+1]=0;//虚部全部为0
//    }
    __HAL_TIM_SET_COUNTER(&htim6,0);//重设TIM6定时器的计数器值
    timeout=0;
    arm_cfft_radix2_f32(&scfft,fft_inputbuf);	//FFT计算（基2）
    time=__HAL_TIM_GET_COUNTER(&htim6)+(uint32_t)timeout*65536;//计算所用时间
    arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);	//把运算结果复数求模得幅值
//    printf("\r\n%d point FFT runtime:%0.3fms\r\n",FFT_LENGTH,time/1000);
    printf("FFT Result:\r\n");
    for(int i=0;i<FFT_LENGTH;i++)
    {
        printf("%f\r\n",fft_outputbuf[i]);
    }

    //k = FFTlength*w/fs fs必须是整数
    THD = sqrt(fft_outputbuf[base_k*2]*fft_outputbuf[base_k*2] + fft_outputbuf[base_k*3]*fft_outputbuf[base_k*3]
            + fft_outputbuf[base_k*4]*fft_outputbuf[base_k*4] + fft_outputbuf[base_k*5]*fft_outputbuf[base_k*5]) / fft_outputbuf[base_k] * 100;
//    printf("THD:%f\r\n",THD);
}
