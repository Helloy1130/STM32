//
// Created by Y on 2020/10/4.
//

#include "ads1118.h"
#include "spi.h"


uint16_t SPI_RW_Reg(uint16_t CofigReg)
{
    uint16_t data;
    uint8_t tx_data[2];
    uint8_t rx_data[2];
    tx_data[0] = (CofigReg>>8) & 0xff;
    tx_data[1] = (CofigReg) & 0xff;

    ADS1118_CS_LOW;

    HAL_SPI_TransmitReceive(&hspi2, tx_data, rx_data, 2, 10);

    ADS1118_CS_HIGH;

    data = ((uint16_t)rx_data[0]<<8) | (rx_data[1]);

    return data;
}

