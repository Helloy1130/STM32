/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include "LED.h"
#include "InFlash.h"
#include "DigitalTube.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint8_t led_speed = 1;//1-10
uint32_t led_control_time = 100;
/* USER CODE END Variables */
/* Definitions for LedTask */
osThreadId_t LedTaskHandle;
const osThreadAttr_t LedTask_attributes = {
  .name = "LedTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for KeyTask */
osThreadId_t KeyTaskHandle;
const osThreadAttr_t KeyTask_attributes = {
  .name = "KeyTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for DigitaTubeTask */
osThreadId_t DigitaTubeTaskHandle;
const osThreadAttr_t DigitaTubeTask_attributes = {
  .name = "DigitaTubeTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartLedTask(void *argument);
void StartKeyTask(void *argument);
void StartDigitaTubeTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of LedTask */
  LedTaskHandle = osThreadNew(StartLedTask, NULL, &LedTask_attributes);

  /* creation of KeyTask */
  KeyTaskHandle = osThreadNew(StartKeyTask, NULL, &KeyTask_attributes);

  /* creation of DigitaTubeTask */
  DigitaTubeTaskHandle = osThreadNew(StartDigitaTubeTask, NULL, &DigitaTubeTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartLedTask */
/**
  * @brief  Function implementing the LedTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartLedTask */
void StartLedTask(void *argument)
{
  /* USER CODE BEGIN StartLedTask */
    uint8_t led_id = 1;

    osDelay(100);
    //读取FLASH地址保存的speed参数
    ReadFlash((uint8_t *) ADDR_FLASH_SECTOR_5, &led_speed, sizeof(led_speed));
    if (led_speed > 10) {
        led_speed = 1;
    }
    /* Infinite loop */
    for (;;) {
            led_control_time = 220 - led_speed * 20;
            turnoffLED();
            selectLED(led_id);
            if (led_id == 8) {
                led_id = 1;
            } else {
                led_id++;
            }
            osDelay(led_control_time);
            osDelay(1);

    }
  /* USER CODE END StartLedTask */
}

/* USER CODE BEGIN Header_StartKeyTask */
/**
* @brief Function implementing the KeyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKeyTask */
void StartKeyTask(void *argument)
{
  /* USER CODE BEGIN StartKeyTask */
    uint8_t key_state;
    /* Infinite loop */
    for (;;) {
        //扫描按键
        key_state = 0;
        if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET) {
            osDelay(10);
            if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET) {
                key_state = 1;
            }
            while (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET) {
                osDelay(1);
            }
        } else if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET) {
            osDelay(10);
            if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET) {
                key_state = 2;
            }
            while (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET) {
                osDelay(1);
            }
        } else if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET) {
            osDelay(10);
            if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET) {
                key_state = 3;
            }
            while (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET) {
                osDelay(1);
            }
        } else if (HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == GPIO_PIN_RESET) {
            osDelay(10);
            if (HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == GPIO_PIN_RESET) {
                key_state = 4;
            }
            while (HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == GPIO_PIN_RESET) {
                osDelay(1);
            }
        } else if (HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin) == GPIO_PIN_SET) {
            osDelay(10);
            if (HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin) == GPIO_PIN_SET) {
                key_state = 5;
            }
            while (HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin) == GPIO_PIN_SET) {
                osDelay(1);
            }
        } else if (HAL_GPIO_ReadPin(KEY6_GPIO_Port, KEY6_Pin) == GPIO_PIN_SET) {
            osDelay(10);
            if (HAL_GPIO_ReadPin(KEY6_GPIO_Port, KEY6_Pin) == GPIO_PIN_SET) {
                key_state = 6;
            }
            while (HAL_GPIO_ReadPin(KEY6_GPIO_Port, KEY6_Pin) == GPIO_PIN_SET) {
                osDelay(1);
            }
        }

        //按键触发控制
        switch (key_state) {
            case 1://跳转程序
                JumpAPP(ADDR_FLASH_SECTOR_6);//跳转到0x08040000执行
                break;

            case 2://减速
                if (led_speed <= 1) {
                    led_speed = 10;
                } else {
                    led_speed--;
                }
                //写入led_speed参数到FLASH地址
                WriteFlash(&led_speed, (uint8_t *) ADDR_FLASH_SECTOR_5, sizeof(led_speed));
                break;

            case 3://加速
                if (led_speed >= 10) {
                    led_speed = 1;
                } else {
                    led_speed++;
                }
                WriteFlash(&led_speed, (uint8_t *) ADDR_FLASH_SECTOR_5, sizeof(led_speed));
                break;

            case 4:
                break;

            case 5:
                break;

            case 6:
                break;

            default:
                break;
        }
        osDelay(1);
    }
  /* USER CODE END StartKeyTask */
}

/* USER CODE BEGIN Header_StartDigitaTubeTask */
/**
* @brief Function implementing the DigitaTubeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDigitaTubeTask */
void StartDigitaTubeTask(void *argument)
{
  /* USER CODE BEGIN StartDigitaTubeTask */
    uint8_t seg[4] = {0};
    /* Infinite loop */
    for (;;) {
        seg[3] = led_speed % 10;
        seg[2] = led_speed / 10;
        for (int i = 0; i < 4; ++i) {
            Write595(i,seg[i],0);
            osDelay(5);
        }
    }
  /* USER CODE END StartDigitaTubeTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
