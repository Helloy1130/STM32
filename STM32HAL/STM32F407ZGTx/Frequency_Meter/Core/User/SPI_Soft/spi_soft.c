//
// Created by Y on 2020/10/26.
//

#include "spi_soft.h"

extern uint32_t fbase_cnt;  //��׼�ź�Ƶ�ʼ���ֵ
extern uint32_t fx_cnt;     //�����ź�Ƶ�ʼ���ֵ
extern uint32_t time_cnt;   //˫ͨ��ͬƵ���źŵ�ʱ��������ֵ
extern uint32_t duty_cnt;   //�����ź�ռ�ձȼ���ֵ
extern uint8_t end_flag;	//������־0xff
extern uint8_t check_flag1;	//У����0x0a
extern uint8_t check_flag2;	//У����0x0a

uint8_t Soft_SPI_Read_Byte(void)
{
    uint8_t i;
    uint8_t start_flag = 0;

    SOFT_SPI_CS_LOW;
    for(i=0;i<8;i++)
    {
        SOFT_SPI_SCK_HIGH;

        SOFT_SPI_SCK_LOW;

        start_flag <<= 1;// ����
        start_flag = start_flag | SOFT_SPI_MISO;// ��������
    }
    if(start_flag == 0x11)//��ʼ����
    {
        for(i=0;i<32;i++)//�����ź�ռ�ձȼ���ֵ
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            duty_cnt <<= 1;// ����
            duty_cnt = duty_cnt | SOFT_SPI_MISO;// ��������
        }
        for(i=0;i<32;i++)//˫ͨ��ͬƵ���źŵ�ʱ��������ֵ
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            time_cnt <<= 1;// ����
            time_cnt = time_cnt | SOFT_SPI_MISO;// ��������
        }
        for(i=0;i<32;i++)//�����ź�Ƶ�ʼ���ֵ
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            fx_cnt <<= 1;// ����
            fx_cnt = fx_cnt | SOFT_SPI_MISO;// ��������
        }
        for(i=0;i<32;i++)//��׼�ź�Ƶ�ʼ���ֵ
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            fbase_cnt <<= 1;// ����
            fbase_cnt = fbase_cnt | SOFT_SPI_MISO;// ��������
        }
        for(i=0;i<8;i++)//������־0xff
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            end_flag <<= 1;// ����
            end_flag = end_flag | SOFT_SPI_MISO;// ��������
        }
        for(i=0;i<8;i++)//У����0x0a
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            check_flag1 <<= 1;// ����
            check_flag1 = check_flag1 | SOFT_SPI_MISO;// ��������
        }
        for(i=0;i<8;i++)//У����0x0b
        {
            SOFT_SPI_SCK_HIGH;

            SOFT_SPI_SCK_LOW;

            check_flag2 <<= 1;// ����
            check_flag2 = check_flag2 | SOFT_SPI_MISO;// ��������
        }
    }
    else start_flag = 0;
    SOFT_SPI_CS_HIGH;
    return start_flag;
}

