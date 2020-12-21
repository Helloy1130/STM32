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
#include <math.h>
#include <stdio.h>
#include "tim.h"
#include "dac.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PI 3.1415926
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for MainTask */
osThreadId_t MainTaskHandle;
const osThreadAttr_t MainTask_attributes = {
  .name = "MainTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void SquareWave(uint16_t *D, float Um, uint16_t cycle);

void TriangleWave(uint16_t *D, float Um, uint16_t cycle);

void SineWave(uint16_t *D, float Um, uint16_t cycle);

void SpecialWave(uint16_t *D);
/* USER CODE END FunctionPrototypes */

void StartMainTask(void *argument);

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
  /* creation of MainTask */
  MainTaskHandle = osThreadNew(StartMainTask, NULL, &MainTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartMainTask */
/**
  * @brief  Function implementing the MainTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMainTask */
void StartMainTask(void *argument)
{
  /* USER CODE BEGIN StartMainTask */
        uint16_t SquareWave_Value[168];
        uint16_t TriangleWave_Value[168];
        uint16_t SineWave_Value[168];
        uint16_t SpecialWave_Value[168];

        SquareWave(SquareWave_Value,3,168);
        TriangleWave(TriangleWave_Value,1.5,168);
        SineWave(SineWave_Value,1.5,168);

        HAL_TIM_Base_Start(&htim2);
        HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)SquareWave_Value, 168, DAC_ALIGN_12B_R);
        HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
  /* USER CODE END StartMainTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/********方波输出表***********/
//cycle :波形表的位数 (0~256)
//Um        :输出电压的峰值(0~3.3)
/*******************************/
void SquareWave(uint16_t *D, float Um, uint16_t cycle) {
    for (int i = 0; i < cycle; i++) {
        if (i < cycle / 2)
            D[i] = (uint16_t) (Um * 4095 / 3.3);
        else
            D[i] = (uint16_t) (0);
    }
}

/********三角波输出表***********/
//cycle :波形表的位数 (0~256)
//Um        :输出电压的峰值(0~1.5)
/*******************************/
void TriangleWave(uint16_t *D, float Um, uint16_t cycle) {
    for (int i = 0; i < cycle; i++) {
        if (i < 23)
            D[i] = (uint16_t) ((Um + i * Um / 24) * 4095 / 3.3);
        else if (i < 72)
            D[i] = (uint16_t) ((2 * Um - (i - 23) * Um / 24) * 4095 / 3.3);
        else
            D[i] = (uint16_t) (((i - 71) * Um / 24) * 4095 / 3.3);
    }
}

/********正弦波输出表***********/
//cycle :波形表的位数 (0~256)
//Um        :输出电压的峰值(0~1.5)
/*******************************/
void SineWave(uint16_t *D, float Um, uint16_t cycle) {
    for (int i = 0; i < cycle; i++) {
        D[i] = (uint16_t) ((Um * sin((1.0 * i / (cycle - 1)) * 2 * PI) + Um) * 4095 / 3.3);
    }
}

//生成特殊波形的数据表
void SpecialWave(uint16_t *D) {
    for (int i = 0; i < 300; i++) {
        if (i < 55)
            D[i] = (uint16_t) ((2 - i / 54.0) * 4095 / 3.3);
        else if (i < 110)
            D[i] = (uint16_t) ((1 + (i - 55) / 54.0) * 4095 / 3.3);
        else if (i < 190)
            D[i] = (uint16_t) (0);
        else if (i < 245)
            D[i] = (uint16_t) (2 * sin((i - 190) / 54.0 * PI / 2) * 4095 / 3.3);
        else if (i < 300)
            D[i] = (uint16_t) ((2 - (i - 245) / 54.0 * 2) * 4095 / 3.3);
    }
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
