//
// Created by Y on 2020/10/26.
//

#include "spi_soft.h"

extern uint32_t fbase_cnt;  //标准信号频率计数值
extern uint32_t fx_cnt;     //待测信号频率计数值
extern uint32_t time_cnt;   //双通道同频率信号的时间间隔计数值
extern uint32_t duty_cnt;   //待测信号占空比计数值
extern uint8_t end_flag;	//结束标志0xff
extern uint8_t check_flag1;	//校验码0x0a
extern uint8_t check_flag2;	//校验码0x0a

uint8_t Soft_SPI_Read_Byte(void)
{
    uint8_t i;
    uint8_t start_flag = 0;

    SOFT_SPI_CS_LOW;
    for(i=0;i<8;i++)
    {
        SOFT_SPI_SCK_HIGH;

        SOFT_SPI_SCK_LOW;

        start_flag <<= 1;// 左移
        start_flag = start_flag | SOFT_SPI_MISO;// 接收数据
    }
    if(start_flag == 0x11)//开始接收
    {
        for(i=0;i<32;i++)//待测信号占空比计数值
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            duty_cnt <<= 1;// 左移
            duty_cnt = duty_cnt | SOFT_SPI_MISO;// 接收数据
        }
        for(i=0;i<32;i++)//双通道同频率信号的时间间隔计数值
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            time_cnt <<= 1;// 左移
            time_cnt = time_cnt | SOFT_SPI_MISO;// 接收数据
        }
        for(i=0;i<32;i++)//待测信号频率计数值
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            fx_cnt <<= 1;// 左移
            fx_cnt = fx_cnt | SOFT_SPI_MISO;// 接收数据
        }
        for(i=0;i<32;i++)//标准信号频率计数值
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            fbase_cnt <<= 1;// 左移
            fbase_cnt = fbase_cnt | SOFT_SPI_MISO;// 接收数据
        }
        for(i=0;i<8;i++)//结束标志0xff
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            end_flag <<= 1;// 左移
            end_flag = end_flag | SOFT_SPI_MISO;// 接收数据
        }
        for(i=0;i<8;i++)//校验码0x0a
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            check_flag1 <<= 1;// 左移
            check_flag1 = check_flag1 | SOFT_SPI_MISO;// 接收数据
        }
        for(i=0;i<8;i++)//校验码0x0b
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            check_flag2 <<= 1;// 左移
            check_flag2 = check_flag2 | SOFT_SPI_MISO;// 接收数据
        }
    }
    else start_flag = 0;
    SOFT_SPI_CS_HIGH;
    return start_flag;
}

