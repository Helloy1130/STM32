//
// Created by Y on 2020/11/3.
//

#include "JY901.h"
#include "usart.h"

#if debug_JY901
#include <stdio.h>
#endif

uint8_t JY901_rx_buf[89];
uint8_t JY901_raw_data[44];
float ax,ay,az;
float wx,wy,wz;
float roll,pitch,yaw;
float Q0,Q1,Q2,Q3;

void readJY901(void)
{
    HAL_UART_Receive(&huart3, JY901_rx_buf, 89, 100);

    for (int i = 1; i < 89; ++i) {
        if(JY901_rx_buf[i] == 0x55 && JY901_rx_buf[i+1] == 0x51) {
            for (int j = i; j < i+44; ++j) {
                JY901_raw_data[j-i] = JY901_rx_buf[j];
#if debug_JY901
                    printf("%x ", JY901_raw_data[j-i]);
#endif
            }
#if debug_JY901
                printf("\r\n");
#endif
            break;
        }
    }
}

void calculateJY901(void)
{
    ax = (((int16_t)JY901_raw_data[3]<<8)|JY901_raw_data[2])*16*9.8/32768.0;
    ay = (((int16_t)JY901_raw_data[5]<<8)|JY901_raw_data[4])*16*9.8/32768.0;
    az = (((int16_t)JY901_raw_data[7]<<8)|JY901_raw_data[6])*16*9.8/32768.0;

    wx = (((int16_t)JY901_raw_data[14]<<8)|JY901_raw_data[13])*2000/32768.0;
    wy = (((int16_t)JY901_raw_data[16]<<8)|JY901_raw_data[15])*2000/32768.0;
    wz = (((int16_t)JY901_raw_data[18]<<8)|JY901_raw_data[17])*2000/32768.0;

    roll = (((int16_t)JY901_raw_data[25]<<8)|JY901_raw_data[24])*180/32768.0;
    pitch = (((int16_t)JY901_raw_data[27]<<8)|JY901_raw_data[26])*180/32768.0;
    yaw = (((int16_t)JY901_raw_data[29]<<8)|JY901_raw_data[28])*180/32768.0;

    Q0 = (((int16_t)JY901_raw_data[36]<<8)|JY901_raw_data[35])/32768.0;
    Q1 = (((int16_t)JY901_raw_data[38]<<8)|JY901_raw_data[37])/32768.0;
    Q2 = (((int16_t)JY901_raw_data[40]<<8)|JY901_raw_data[39])/32768.0;
    Q3 = (((int16_t)JY901_raw_data[42]<<8)|JY901_raw_data[41])/32768.0;
}


