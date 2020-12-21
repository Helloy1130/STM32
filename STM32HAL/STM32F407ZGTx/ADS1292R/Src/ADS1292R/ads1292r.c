#include "ads1292r.h"
#include "spi.h"
#include <stdio.h>

#define DEBUG_ADS1292 1

uint8_t ads1292_data_buf[9];    //数据缓冲区
uint8_t ads1292_recive_flag=0;	//数据读取完成标志

//通过SPI总线与ADS1292通信
static uint8_t ads1292_SPI_RW(uint8_t data)
{
    uint8_t rx_data = 0;
    HAL_SPI_TransmitReceive(&hspi2, &data, &rx_data, 1, 10);
    return rx_data;
}

static void ads1292_SPI_Command_Data(unsigned char data_in)
{
    ADS1292R_CS_LOW;
    HAL_Delay(2);
    ADS1292R_CS_HIGH;
    HAL_Delay(2);
    ADS1292R_CS_LOW;
    HAL_Delay(2);
    ads1292_SPI_RW(data_in);
    HAL_Delay(2);
    ADS1292R_CS_HIGH;
}

static uint8_t ads1292_Read_Data(uint8_t *data)
{
    ADS1292R_CS_LOW;

    for (int i = 0; i < 9; ++i)
    {
        *data=ads1292_SPI_RW(0xFF);
        data++;
    }

    ADS1292R_CS_HIGH;

    return 0;
}

static void ADS1292_WR_REGS(uint8_t reg,uint8_t len,uint8_t *data)
{
    uint8_t i;
    ADS1292R_CS_LOW;
    HAL_Delay(1);
    ads1292_SPI_RW(reg);
    HAL_Delay(1);
    ads1292_SPI_RW(len-1);
    if(reg&0x40) //写
    {
        for(i=0;i<len;i++)
        {
            HAL_Delay(1);
            ads1292_SPI_RW(*data);
            data++;
        }
    }
    else //读
    {
        for(i=0;i<len;i++)
        {
            HAL_Delay(1);
            *data = ads1292_SPI_RW(0);
            data++;
        }
    }
    HAL_Delay(1);
    ADS1292R_CS_HIGH;
}

static void ads1292_Reset(void)
{
    ADS1292R_PWDN_HIGH;
    HAL_Delay(100);					// Wait 100 mSec
    ADS1292R_PWDN_LOW;
    HAL_Delay(100);
    ADS1292R_PWDN_HIGH;
    HAL_Delay(100);
}

static void ads1292_Disable_Start(void)
{
    ADS1292R_START_LOW;
    HAL_Delay(20);
}

static void ads1292_Enable_Start(void)
{
    ADS1292R_START_HIGH;
    HAL_Delay(20);
}

static void ads1292_Hard_Stop (void)
{
    ADS1292R_START_LOW;
    HAL_Delay(100);
}

static void ads1292_Start_Data_Conv_Command (void)
{
    ads1292_SPI_Command_Data(START);					// Send 0x08 to the ADS1x9x
}

static void ads1292_Soft_Stop (void)
{
    ads1292_SPI_Command_Data(STOP);                   // Send 0x0A to the ADS1x9x
}

static void ads1292_Start_Read_Data_Continuous (void)
{
    ads1292_SPI_Command_Data(RDATAC);					// Send 0x10 to the ADS1x9x
}

static void ads1292_Stop_Read_Data_Continuous (void)
{
    ads1292_SPI_Command_Data(SDATAC);					// Send 0x11 to the ADS1x9x
}

uint8_t ADS1292_REG[12];		//ads1292寄存器数组
//设置寄存器数组
static void ADS1292_SET_REGBUFF(void)
{
    ADS1292_REG[ADS1292_REG_CONFIG1] =	0x00; //Set sampling rate to 125 SPS

    ADS1292_REG[ADS1292_REG_CONFIG2] =	0b10100000;//Lead-off comp off, test signal disabled

    ADS1292_REG[ADS1292_REG_LOFF] =	0b00010000;//Lead-off defaults

    ADS1292_REG[ADS1292_REG_CH1SET] =	0b01000000;//Ch 1 enabled, gain 6, connected to electrode in

    ADS1292_REG[ADS1292_REG_CH2SET] =	0b01100000;//Ch 2 enabled, gain 6, connected to electrode in

    ADS1292_REG[ADS1292_REG_RLDSENS] = 0b00101100;//RLD settings: fmod/16, RLD enabled, RLD inputs from Ch2 only

    ADS1292_REG[ADS1292_REG_LOFFSENS] = 0x00;//LOFF settings: all disabled

    //Skip register 8, LOFF Settings default
    ADS1292_REG[ADS1292_REG_RESP1] = 0b11110010;//Respiration: MOD/DEMOD turned only, phase 0

    ADS1292_REG[ADS1292_REG_RESP2] = 0b00000011;//Respiration: Calib OFF, respiration freq defaults
}

void ads1292_Init(void)
{
    ads1292_Reset();
    HAL_Delay(100);
    ads1292_Disable_Start();
    ads1292_Enable_Start();

    ads1292_Hard_Stop();
    ads1292_Start_Data_Conv_Command();
    ads1292_Soft_Stop();
    HAL_Delay(50);
    ads1292_Stop_Read_Data_Continuous();					// SDATAC command
    HAL_Delay(300);

    ADS1292_SET_REGBUFF();//设置寄存器数组

    ADS1292_WR_REGS(WREG|ADS1292_REG_CONFIG1,11,ADS1292_REG+1);//写入寄存器

#if DEBUG_ADS1292
    uint8_t REG_Cache[12];//读取寄存器
    ADS1292_WR_REGS(RREG|ADS1292_REG_ID,12,REG_Cache);
    printf("read REG:\r\n");
    uint8_t res = 0;
    for(int i=0;i<12;i++)//检查寄存器
    {
        if(ADS1292_REG[i] != REG_Cache[i])
        {
            if(i!= 0 && i!=8 && i != 11)	//0 8 和11是ID 导联脱落和GPIO相关
                res=1;
            else
                continue;
        }
        printf("%d %x\r\n",i,REG_Cache[i]); //读到的数据
    }

    if(res == 0)
        printf("REG write success\r\n");
    else
        printf("REG write err\r\n");
#endif

    ads1292_Start_Read_Data_Continuous();
    HAL_Delay(10);
    ads1292_Enable_Start();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(GPIO_Pin);
    /* NOTE: This function Should not be modified, when the callback is needed,
             the HAL_GPIO_EXTI_Callback could be implemented in the user file
     */
    switch(GPIO_Pin)
    {
        case ADS1292R_DRDY_Pin:
            if(ADS1292R_DRDY==0) {
                ads1292_Read_Data(ads1292_data_buf);//数据存到9字节缓冲区
                ads1292_recive_flag=1;
            }
            break;
    }
}
