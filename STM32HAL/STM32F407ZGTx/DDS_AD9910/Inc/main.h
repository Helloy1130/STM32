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
#define AD9910_CS_Pin GPIO_PIN_5
#define AD9910_CS_GPIO_Port GPIOE
#define AD9910_DRCTL_Pin GPIO_PIN_6
#define AD9910_DRCTL_GPIO_Port GPIOE
#define GT_RST_Pin GPIO_PIN_13
#define GT_RST_GPIO_Port GPIOC
#define AD9910_DRHOLD_Pin GPIO_PIN_0
#define AD9910_DRHOLD_GPIO_Port GPIOF
#define AD9910_DROVER_Pin GPIO_PIN_1
#define AD9910_DROVER_GPIO_Port GPIOF
#define AD9910_MASREST_Pin GPIO_PIN_2
#define AD9910_MASREST_GPIO_Port GPIOF
#define AD9910_OSK_Pin GPIO_PIN_3
#define AD9910_OSK_GPIO_Port GPIOF
#define AD9910_PROFILE0_Pin GPIO_PIN_4
#define AD9910_PROFILE0_GPIO_Port GPIOF
#define AD9910_PROFILE1_Pin GPIO_PIN_5
#define AD9910_PROFILE1_GPIO_Port GPIOF
#define AD9910_PROFILE2_Pin GPIO_PIN_6
#define AD9910_PROFILE2_GPIO_Port GPIOF
#define AD9910_PWR_Pin GPIO_PIN_7
#define AD9910_PWR_GPIO_Port GPIOF
#define AD9910_SCLK_Pin GPIO_PIN_8
#define AD9910_SCLK_GPIO_Port GPIOF
#define LED0_Pin GPIO_PIN_9
#define LED0_GPIO_Port GPIOF
#define AD9910_SDIO_Pin GPIO_PIN_0
#define AD9910_SDIO_GPIO_Port GPIOC
#define AD9910_UPDAT_Pin GPIO_PIN_1
#define AD9910_UPDAT_GPIO_Port GPIOC
#define CT_IIC_SCL_Pin GPIO_PIN_0
#define CT_IIC_SCL_GPIO_Port GPIOB
#define GT_INT_Pin GPIO_PIN_1
#define GT_INT_GPIO_Port GPIOB
#define CT_IIC_SDA_Pin GPIO_PIN_11
#define CT_IIC_SDA_GPIO_Port GPIOF
#define LCD_BL_Pin GPIO_PIN_15
#define LCD_BL_GPIO_Port GPIOB
#define MY_IIC_SCL_Pin GPIO_PIN_8
#define MY_IIC_SCL_GPIO_Port GPIOB
#define MY_IIC_SDA_Pin GPIO_PIN_9
#define MY_IIC_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
