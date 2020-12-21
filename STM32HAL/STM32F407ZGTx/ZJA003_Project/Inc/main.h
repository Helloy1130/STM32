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
#define FPGA_DATA15_Pin GPIO_PIN_6
#define FPGA_DATA15_GPIO_Port GPIOE
#define GT_RST_Pin GPIO_PIN_13
#define GT_RST_GPIO_Port GPIOC
#define FPGA_DATA14_Pin GPIO_PIN_0
#define FPGA_DATA14_GPIO_Port GPIOF
#define FPGA_DATA13_Pin GPIO_PIN_1
#define FPGA_DATA13_GPIO_Port GPIOF
#define FPGA_DATA12_Pin GPIO_PIN_2
#define FPGA_DATA12_GPIO_Port GPIOF
#define FPGA_DATA11_Pin GPIO_PIN_3
#define FPGA_DATA11_GPIO_Port GPIOF
#define FPGA_DATA10_Pin GPIO_PIN_4
#define FPGA_DATA10_GPIO_Port GPIOF
#define FPGA_DATA9_Pin GPIO_PIN_5
#define FPGA_DATA9_GPIO_Port GPIOF
#define FPGA_DATA8_Pin GPIO_PIN_6
#define FPGA_DATA8_GPIO_Port GPIOF
#define FPGA_DATA7_Pin GPIO_PIN_7
#define FPGA_DATA7_GPIO_Port GPIOF
#define FPGA_DATA6_Pin GPIO_PIN_8
#define FPGA_DATA6_GPIO_Port GPIOF
#define LED0_Pin GPIO_PIN_9
#define LED0_GPIO_Port GPIOF
#define FPGA_DATA5_Pin GPIO_PIN_0
#define FPGA_DATA5_GPIO_Port GPIOC
#define FPGA_DATA4_Pin GPIO_PIN_1
#define FPGA_DATA4_GPIO_Port GPIOC
#define FPGA_DATA3_Pin GPIO_PIN_2
#define FPGA_DATA3_GPIO_Port GPIOC
#define FPGA_DATA2_Pin GPIO_PIN_3
#define FPGA_DATA2_GPIO_Port GPIOC
#define FPGA_DATA_CS_Pin GPIO_PIN_1
#define FPGA_DATA_CS_GPIO_Port GPIOA
#define FPGA_DATA1_Pin GPIO_PIN_2
#define FPGA_DATA1_GPIO_Port GPIOA
#define FPGA_DATA0_Pin GPIO_PIN_3
#define FPGA_DATA0_GPIO_Port GPIOA
#define CT_IIC_SCL_Pin GPIO_PIN_0
#define CT_IIC_SCL_GPIO_Port GPIOB
#define GT_INT_Pin GPIO_PIN_1
#define GT_INT_GPIO_Port GPIOB
#define CT_IIC_SDA_Pin GPIO_PIN_11
#define CT_IIC_SDA_GPIO_Port GPIOF
#define LCD_BL_Pin GPIO_PIN_15
#define LCD_BL_GPIO_Port GPIOB
#define CONTROL_D5_Pin GPIO_PIN_6
#define CONTROL_D5_GPIO_Port GPIOD
#define CONTROL_D6_Pin GPIO_PIN_7
#define CONTROL_D6_GPIO_Port GPIOD
#define CONTROL_D1_Pin GPIO_PIN_14
#define CONTROL_D1_GPIO_Port GPIOG
#define CONTROL_D2_Pin GPIO_PIN_3
#define CONTROL_D2_GPIO_Port GPIOB
#define CONTROL_D3_Pin GPIO_PIN_5
#define CONTROL_D3_GPIO_Port GPIOB
#define CONTROL_D4_Pin GPIO_PIN_7
#define CONTROL_D4_GPIO_Port GPIOB
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
