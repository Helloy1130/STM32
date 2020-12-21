/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY2_Pin GPIO_PIN_2
#define KEY2_GPIO_Port GPIOE
#define KEY3_Pin GPIO_PIN_3
#define KEY3_GPIO_Port GPIOE
#define KEY4_Pin GPIO_PIN_4
#define KEY4_GPIO_Port GPIOE
#define KEY5_Pin GPIO_PIN_5
#define KEY5_GPIO_Port GPIOE
#define KEY6_Pin GPIO_PIN_6
#define KEY6_GPIO_Port GPIOE
#define OLED_I2C_SCL_Pin GPIO_PIN_6
#define OLED_I2C_SCL_GPIO_Port GPIOB
#define OLED_I2C_SDA_Pin GPIO_PIN_7
#define OLED_I2C_SDA_GPIO_Port GPIOB
#define DS18B20_DQ_Pin GPIO_PIN_0
#define DS18B20_DQ_GPIO_Port GPIOE
#define KEY1_Pin GPIO_PIN_1
#define KEY1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
#define MAX_BUF_LEN 256
//ADC_MODE 1:��ѯ 2:�ж� 3:DMA
#define ADC_MODE 3
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
