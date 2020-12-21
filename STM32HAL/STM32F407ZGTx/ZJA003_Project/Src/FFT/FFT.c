//
// Created by Y on 2020/10/10.
//

#include "FFT.h"
#include "arm_math.h"
#include "tim.h"
#include <stdio.h>

float THD;
float time;

float fft_inputbuf[FFT_LENGTH*2];	//FFT��������
float fft_outputbuf[FFT_LENGTH];	//FFT�������

uint8_t timeout;
arm_cfft_radix2_instance_f32 scfft;

void FFT_Init(void)
{
    arm_cfft_radix2_init_f32(&scfft,FFT_LENGTH,0,1);//��ʼ��scfft�ṹ�壬�趨FFT��ز���
}

void FFT_Process(void)
{
//    for(int i=0;i<FFT_LENGTH;i++)//�����ź�����
//    {
////        fft_inputbuf[2*i]=arm_sin_f32(2*PI*i/FFT_LENGTH);	//���������ź�ʵ��
//        fft_inputbuf[2*i]=Wave_Data[i];	//���������ź�ʵ��
//        fft_inputbuf[2*i+1]=0;//�鲿ȫ��Ϊ0
//    }
    __HAL_TIM_SET_COUNTER(&htim6,0);//����TIM6��ʱ���ļ�����ֵ
    timeout=0;
    arm_cfft_radix2_f32(&scfft,fft_inputbuf);	//FFT���㣨��2��
    time=__HAL_TIM_GET_COUNTER(&htim6)+(uint32_t)timeout*65536;//��������ʱ��
    arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);	//��������������ģ�÷�ֵ
//    printf("\r\n%d point FFT runtime:%0.3fms\r\n",FFT_LENGTH,time/1000);
    printf("FFT Result:\r\n");
    for(int i=0;i<FFT_LENGTH;i++)
    {
        printf("%f\r\n",fft_outputbuf[i]);
    }

    //k = FFTlength*w/fs fs����������
    THD = sqrt(fft_outputbuf[base_k*2]*fft_outputbuf[base_k*2] + fft_outputbuf[base_k*3]*fft_outputbuf[base_k*3]
            + fft_outputbuf[base_k*4]*fft_outputbuf[base_k*4] + fft_outputbuf[base_k*5]*fft_outputbuf[base_k*5]) / fft_outputbuf[base_k] * 100;
//    printf("THD:%f\r\n",THD);
}
