//
// Created by Y on 2020/10/7.
//

#include "TLV5638.h"
#include "spi.h"

void TLV5638_send(uint8_t mode, uint16_t data)
{
    uint16_t temp;
    uint8_t tx_data[2];

    temp = data & 0x0fff;

    temp |= ((uint16_t)mode<<12);      //组合成16位整型控制码

    tx_data[0] = (temp>>8) & 0xff;
    tx_data[1] = (temp) & 0xff;

    TLV5638_CS_LOW;

    HAL_SPI_Transmit(&hspi2, tx_data, 2, 10);

    TLV5638_CS_HIGH;

}

void TLV5638_init(void)
{
    TLV5638_send(0x0D,0x0002);//Write data to control register,REFERENCE = 2.048V

    TLV5638_send(0x05,2047);//Write data for DAC B to BUFFER. B = 2.048V
    TLV5638_send(0x0C,4095);//Write new DAC A value and update DAC A and B simultaneously. A = 4.096V
}

void TLV5638_set(uint16_t DAC_A, uint16_t DAC_B)
{
    TLV5638_send(0x05,DAC_B);//Write data for DAC B to BUFFER
    TLV5638_send(0x0C,DAC_A);//Write new DAC A value and update DAC A and B simultaneously
}
