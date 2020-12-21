//
// Created by Y on 2020/10/7.
//

#ifndef TLV5638_TLV5638_H
#define TLV5638_TLV5638_H

#include "main.h"

#define TLV5638_CS_HIGH		HAL_GPIO_WritePin(TLV5638_CS_GPIO_Port, TLV5638_CS_Pin,GPIO_PIN_SET)
#define TLV5638_CS_LOW		HAL_GPIO_WritePin(TLV5638_CS_GPIO_Port, TLV5638_CS_Pin,GPIO_PIN_RESET)

void TLV5638_send(uint8_t mode, uint16_t data);
void TLV5638_init(void);
void TLV5638_set(uint16_t DAC_A, uint16_t DAC_B);

#endif //TLV5638_TLV5638_H
