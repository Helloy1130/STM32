//
// Created by Y on 2020/10/6.
//

#include "DAC7811.h"
#include "spi.h"

void DAC7811_send(uint8_t mode, uint16_t data)
{
    uint16_t temp;
    uint8_t tx_data[2];

    temp = data & 0x0fff;

    temp |= ((uint16_t)mode<<12);      //组合成16位整型控制码

    tx_data[0] = (temp>>8) & 0xff;
    tx_data[1] = (temp) & 0xff;

    DAC7811_SYNC_LOW;//开始写入数据

    HAL_SPI_Transmit(&hspi2, tx_data, 2, 10);

    DAC7811_SYNC_HIGH;

}
