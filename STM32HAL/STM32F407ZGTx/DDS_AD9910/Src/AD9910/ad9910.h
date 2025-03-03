//
// Created by Y on 2020/10/3.
//

#ifndef DDS_AD9910_AD9910_H
#define DDS_AD9910_AD9910_H

#include "main.h"

#define AD9910_PROFILE0_HIGH	    HAL_GPIO_WritePin(AD9910_PROFILE0_GPIO_Port, AD9910_PROFILE0_Pin, GPIO_PIN_SET)
#define AD9910_PROFILE0_LOW		    HAL_GPIO_WritePin(AD9910_PROFILE0_GPIO_Port, AD9910_PROFILE0_Pin, GPIO_PIN_RESET)
#define AD9910_PROFILE1_HIGH	    HAL_GPIO_WritePin(AD9910_PROFILE1_GPIO_Port, AD9910_PROFILE1_Pin, GPIO_PIN_SET)
#define AD9910_PROFILE1_LOW		    HAL_GPIO_WritePin(AD9910_PROFILE1_GPIO_Port, AD9910_PROFILE1_Pin, GPIO_PIN_RESET)
#define AD9910_PROFILE2_HIGH	    HAL_GPIO_WritePin(AD9910_PROFILE2_GPIO_Port, AD9910_PROFILE2_Pin, GPIO_PIN_SET)
#define AD9910_PROFILE2_LOW		    HAL_GPIO_WritePin(AD9910_PROFILE2_GPIO_Port, AD9910_PROFILE2_Pin, GPIO_PIN_RESET)
#define AD9910_CS_HIGH		        HAL_GPIO_WritePin(AD9910_CS_GPIO_Port, AD9910_CS_Pin, GPIO_PIN_SET)
#define AD9910_CS_LOW			    HAL_GPIO_WritePin(AD9910_CS_GPIO_Port, AD9910_CS_Pin, GPIO_PIN_RESET)
#define AD9910_MASREST_HIGH		    HAL_GPIO_WritePin(AD9910_MASREST_GPIO_Port, AD9910_MASREST_Pin, GPIO_PIN_SET)
#define AD9910_MASREST_LOW			HAL_GPIO_WritePin(AD9910_MASREST_GPIO_Port, AD9910_MASREST_Pin, GPIO_PIN_RESET)
#define AD9910_SCLK_HIGH		    HAL_GPIO_WritePin(AD9910_SCLK_GPIO_Port, AD9910_SCLK_Pin, GPIO_PIN_SET)
#define AD9910_SCLK_LOW			    HAL_GPIO_WritePin(AD9910_SCLK_GPIO_Port, AD9910_SCLK_Pin, GPIO_PIN_RESET)
#define AD9910_DRCTL_HIGH		    HAL_GPIO_WritePin(AD9910_DRCTL_GPIO_Port, AD9910_DRCTL_Pin, GPIO_PIN_SET)
#define AD9910_DRCTL_LOW			HAL_GPIO_WritePin(AD9910_DRCTL_GPIO_Port, AD9910_DRCTL_Pin, GPIO_PIN_RESET)
#define AD9910_PWR_HIGH		        HAL_GPIO_WritePin(AD9910_PWR_GPIO_Port, AD9910_PWR_Pin, GPIO_PIN_SET)
#define AD9910_PWR_LOW			    HAL_GPIO_WritePin(AD9910_PWR_GPIO_Port, AD9910_PWR_Pin, GPIO_PIN_RESET)
#define AD9910_SDIO_HIGH		    HAL_GPIO_WritePin(AD9910_SDIO_GPIO_Port, AD9910_SDIO_Pin, GPIO_PIN_SET)
#define AD9910_SDIO_LOW			    HAL_GPIO_WritePin(AD9910_SDIO_GPIO_Port, AD9910_SDIO_Pin, GPIO_PIN_RESET)
#define AD9910_DRHOLD_HIGH		    HAL_GPIO_WritePin(AD9910_DRHOLD_GPIO_Port, AD9910_DRHOLD_Pin, GPIO_PIN_SET)
#define AD9910_DRHOLD_LOW			HAL_GPIO_WritePin(AD9910_DRHOLD_GPIO_Port, AD9910_DRHOLD_Pin, GPIO_PIN_RESET)
#define AD9910_DROVER_HIGH		    HAL_GPIO_WritePin(AD9910_DROVER_GPIO_Port, AD9910_DROVER_Pin, GPIO_PIN_SET)
#define AD9910_DROVER_LOW			HAL_GPIO_WritePin(AD9910_DROVER_GPIO_Port, AD9910_DROVER_Pin, GPIO_PIN_RESET)
#define AD9910_UPDAT_HIGH		    HAL_GPIO_WritePin(AD9910_UPDAT_GPIO_Port, AD9910_UPDAT_Pin, GPIO_PIN_SET)
#define AD9910_UPDAT_LOW			HAL_GPIO_WritePin(AD9910_UPDAT_GPIO_Port, AD9910_UPDAT_Pin, GPIO_PIN_RESET)
#define AD9910_OSK_HIGH		        HAL_GPIO_WritePin(AD9910_OSK_GPIO_Port, AD9910_OSK_Pin, GPIO_PIN_SET)
#define AD9910_OSK_LOW			    HAL_GPIO_WritePin(AD9910_OSK_GPIO_Port, AD9910_OSK_Pin, GPIO_PIN_RESET)

//void AD9910WriteByte (unsigned char commond, unsigned char *databyte,unsigned char lengh);
void ad9910_init(void);
void Freq_Amp_set(uint32_t Freq,float Amp);					//дƵ��
//void Write_Amplitude(bit Channel,unsigned int Amplitude);	//д����

#endif //DDS_AD9910_AD9910_H
