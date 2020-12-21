//
// Created by Y on 2020/9/28.
//

#ifndef ADS1292R_FIR_FILTER_H
#define ADS1292R_FIR_FILTER_H

#include "main.h"
#include "arm_math.h"

#define LENGTH_SAMPLES       500   /* �������� */
#define BLOCK_SIZE           500   /* ����һ��arm_fir_f32����Ĳ�������� */
#define NUM_TAPS             161    /* �˲���ϵ������ */

void arm_fir_f32_init(void);
void arm_fir_f32_lp(float32_t *Input_f32, float32_t *Output);

#endif //ADS1292R_FIR_FILTER_H
