//
// Created by Y on 2020/10/10.
//

#ifndef ZJA003_PROJECT_FFT_H
#define ZJA003_PROJECT_FFT_H

#include "main.h"

#define FFT_LENGTH		(512) 		//FFT³¤¶È
#define Fs              (64000)

extern uint32_t  base_k;

void FFT_Init(void);
void FFT_Process(void);

#endif //ZJA003_PROJECT_FFT_H
