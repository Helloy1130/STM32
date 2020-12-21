//
// Created by Y on 2020/9/28.
//

#ifndef ADS1292R_FIR_FILTER_H
#define ADS1292R_FIR_FILTER_H

#include "main.h"
#include "arm_math.h"

#define LENGTH_SAMPLES       500   /* 采样点数 */
#define BLOCK_SIZE           500   /* 调用一次arm_fir_f32处理的采样点个数 */
#define NUM_TAPS             161    /* 滤波器系数个数 */

void arm_fir_f32_init(void);
void arm_fir_f32_lp(float32_t *Input_f32, float32_t *Output);

#endif //ADS1292R_FIR_FILTER_H
