//
// Created by Y on 2020/10/2.
//

#ifndef ADS1292R_ECG_PROCESS_H
#define ADS1292R_ECG_PROCESS_H

#include "main.h"

extern uint8_t DataPacketHeader[13];
extern volatile uint8_t global_HeartRate;

void ECG_Process(void);

#endif //ADS1292R_ECG_PROCESS_H
