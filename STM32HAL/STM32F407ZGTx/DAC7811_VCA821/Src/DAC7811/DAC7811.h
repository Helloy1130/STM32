//
// Created by Y on 2020/10/6.
//

#ifndef DAC7811_VCA821_DAC7811_H
#define DAC7811_VCA821_DAC7811_H

#include "main.h"

#define DAC7811_SYNC_HIGH		HAL_GPIO_WritePin(DAC7811_SYNC_GPIO_Port, DAC7811_SYNC_Pin,GPIO_PIN_SET)
#define DAC7811_SYNC_LOW		HAL_GPIO_WritePin(DAC7811_SYNC_GPIO_Port, DAC7811_SYNC_Pin,GPIO_PIN_RESET)

void DAC7811_send(uint8_t mode, uint16_t data);

#endif //DAC7811_VCA821_DAC7811_H
