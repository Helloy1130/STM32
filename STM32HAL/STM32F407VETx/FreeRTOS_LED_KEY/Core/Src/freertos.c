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
#include <stdio.h>
#include <stdbool.h>
#include "tim.h"
#include "LED.h"
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
uint8_t led_id = 1;
bool led_switch = false;
uint32_t led_control_time = 200;
uint8_t key_state = 0;
uint8_t key_buf = 0;
bool beep_switch = false;
/* USER CODE END Variables */
/* Definitions for ledTask */
osThreadId_t ledTaskHandle;
const osThreadAttr_t ledTask_attributes = {
  .name = "ledTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for keyTask */
osThreadId_t keyTaskHandle;
const osThreadAttr_t keyTask_attributes = {
  .name = "keyTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for beepTask */
osThreadId_t beepTaskHandle;
const osThreadAttr_t beepTask_attributes = {
  .name = "beepTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for digitaTubeTask */
osThreadId_t digitaTubeTaskHandle;
const osThreadAttr_t digitaTubeTask_attributes = {
  .name = "digitaTubeTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for temporaryTask */
osThreadId_t temporaryTaskHandle;
const osThreadAttr_t temporaryTask_attributes = {
  .name = "temporaryTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartLedTask(void *argument);
void StartKeyTask(void *argument);
void StartBeepTask(void *argument);
void StartDigitaTubeTask(void *argument);
void StartTemporaryTask(void *argument);

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
  /* creation of ledTask */
  ledTaskHandle = osThreadNew(StartLedTask, NULL, &ledTask_attributes);

  /* creation of keyTask */
  keyTaskHandle = osThreadNew(StartKeyTask, NULL, &keyTask_attributes);

  /* creation of beepTask */
  beepTaskHandle = osThreadNew(StartBeepTask, NULL, &beepTask_attributes);

  /* creation of digitaTubeTask */
  digitaTubeTaskHandle = osThreadNew(StartDigitaTubeTask, NULL, &digitaTubeTask_attributes);

  /* creation of temporaryTask */
  temporaryTaskHandle = osThreadNew(StartTemporaryTask, NULL, &temporaryTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
    osThreadSuspend(temporaryTaskHandle);//挂起临时任务
    osThreadTerminate(temporaryTaskHandle);//删除临时任务
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartLedTask */
/**
  * @brief  Function implementing the ledTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartLedTask */
void StartLedTask(void *argument)
{
  /* USER CODE BEGIN StartLedTask */
  /* Infinite loop */
  for(;;)
  {
      if (led_switch){
          turnoffLED();
          selectLED(led_id);
          if(led_id==8){
              led_id = 1;
          }
          else{
              led_id++;
          }
          osDelay(led_control_time);
      }
      else {
          osDelay(1);
      }
  }
  /* USER CODE END StartLedTask */
}

/* USER CODE BEGIN Header_StartKeyTask */
/**
* @brief Function implementing the keyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKeyTask */
void StartKeyTask(void *argument)
{
  /* USER CODE BEGIN StartKeyTask */
  /* Infinite loop */
  for(;;)
  {
      //扫描按键
      key_state = 0;
      if (HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==GPIO_PIN_RESET){
          osDelay(10);
          if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==GPIO_PIN_RESET)
          {
              key_state = 1;
          }
          while(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==GPIO_PIN_RESET)
          {
              osDelay(1);
          }
      }
      else if (HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==GPIO_PIN_RESET){
          osDelay(10);
          if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==GPIO_PIN_RESET)
          {
              key_state = 2;
          }
          while(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==GPIO_PIN_RESET)
          {
              osDelay(1);
          }
      }
      else if (HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==GPIO_PIN_RESET){
          osDelay(10);
          if(HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==GPIO_PIN_RESET)
          {
              key_state = 3;
          }
          while(HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==GPIO_PIN_RESET)
          {
              osDelay(1);
          }
      }
      else if (HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==GPIO_PIN_RESET){
          osDelay(10);
          if(HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==GPIO_PIN_RESET)
          {
              key_state = 4;
          }
          while(HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==GPIO_PIN_RESET)
          {
              osDelay(1);
          }
      }
      else if (HAL_GPIO_ReadPin(KEY5_GPIO_Port,KEY5_Pin)==GPIO_PIN_SET){
          osDelay(10);
          if(HAL_GPIO_ReadPin(KEY5_GPIO_Port,KEY5_Pin)==GPIO_PIN_SET)
          {
              key_state = 5;
          }
          while(HAL_GPIO_ReadPin(KEY5_GPIO_Port,KEY5_Pin)==GPIO_PIN_SET)
          {
              osDelay(1);
          }
      }
      //按键触发控制
      switch (key_state) {
          case 1://启动流水灯
              led_switch=true;
              __HAL_TIM_SET_AUTORELOAD(&htim3,299);
              key_buf = 1;
              break;

          case 2://减速流水灯
              if(led_control_time<400)led_control_time+=10;
              else led_control_time = 400;
              __HAL_TIM_SET_AUTORELOAD(&htim3,399);
              key_buf = 2;
              break;

          case 3://加速流水灯
              if(led_control_time>10) led_control_time-=10;
              else led_control_time = 10;
              __HAL_TIM_SET_AUTORELOAD(&htim3,499);
              key_buf = 3;
              break;

          case 4://暂停流水灯
              led_switch=false;
              __HAL_TIM_SET_AUTORELOAD(&htim3,599);
              key_buf = 4;
              break;

          case 5:
              osThreadSuspend(digitaTubeTaskHandle);//挂起数码管显示任务
              temporaryTaskHandle = osThreadNew(StartTemporaryTask, NULL, &temporaryTask_attributes);//新建临时任务
              break;

          default:
              break;
      }

      if(key_state) {
          beep_switch = true;
          printf("led_switch = %d,led_control_time = %d\r\n",led_switch,(int)led_control_time);
      }
      osDelay(1);
  }
  /* USER CODE END StartKeyTask */
}

/* USER CODE BEGIN Header_StartBeepTask */
/**
* @brief Function implementing the beepTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBeepTask */
void StartBeepTask(void *argument)
{
  /* USER CODE BEGIN StartBeepTask */
  /* Infinite loop */
  for(;;)
  {
      if (beep_switch) {
          HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);//使能输出
          osDelay(100);
          HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);//使能输出
          beep_switch = false;
      }
      else {
          osDelay(1);
      }
  }
  /* USER CODE END StartBeepTask */
}

/* USER CODE BEGIN Header_StartDigitaTubeTask */
/**
* @brief Function implementing the digitaTubeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDigitaTubeTask */
void StartDigitaTubeTask(void *argument)
{
  /* USER CODE BEGIN StartDigitaTubeTask */
  /* Infinite loop */
  for(;;)
  {
      Write595(0,key_buf,0);
      osDelay(1);
  }
  /* USER CODE END StartDigitaTubeTask */
}

/* USER CODE BEGIN Header_StartTemporaryTask */
/**
* @brief Function implementing the temporaryTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTemporaryTask */
void StartTemporaryTask(void *argument)
{
  /* USER CODE BEGIN StartTemporaryTask */
  /* Infinite loop */
  for(;;)
  {
      Write595(0,8,0);
      osDelay(300);
      Write595(1,8,0);
      osDelay(300);
      Write595(2,8,0);
      osDelay(300);
      Write595(3,8,0);
      osDelay(300);
      osThreadResume(digitaTubeTaskHandle);//开启数码管显示任务
      osThreadTerminate(temporaryTaskHandle);//删除临时任务
  }
  /* USER CODE END StartTemporaryTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
