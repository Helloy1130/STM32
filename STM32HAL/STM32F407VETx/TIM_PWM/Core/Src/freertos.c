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
uint8_t beep_switch = 0;

bool led_breathe = false;
int light = 0;

uint32_t capture_buf[3] = {0};  //存放计数值
uint8_t capture_cnt = 0;        //状态标志位
uint32_t high_time;             //高电平时间
int key6_cnt = 0;               //按键动作次数

extern int tim_compare;
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
/* Definitions for BeepTask */
osThreadId_t BeepTaskHandle;
const osThreadAttr_t BeepTask_attributes = {
  .name = "BeepTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for CaptureTask */
osThreadId_t CaptureTaskHandle;
const osThreadAttr_t CaptureTask_attributes = {
  .name = "CaptureTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartLedTask(void *argument);
void StartKeyTask(void *argument);
void StartBeepTask(void *argument);
void StartCaptureTask(void *argument);

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

  /* creation of BeepTask */
  BeepTaskHandle = osThreadNew(StartBeepTask, NULL, &BeepTask_attributes);

  /* creation of CaptureTask */
  CaptureTaskHandle = osThreadNew(StartCaptureTask, NULL, &CaptureTask_attributes);

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
#if led_use_pwm
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
#else
    HAL_TIM_Base_Start_IT(&htim3);
#endif
  /* Infinite loop */
  for(;;)
  {
      if(led_breathe == true){
          if (light > 100){
#if led_use_pwm
              __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 200 - light);
              __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 200 - light);
              __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 200 - light);
              __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 200 - light);
#else
              tim_compare = 200 - light;
#endif
          }
          else {
#if led_use_pwm
              __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, light);
              __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, light);
              __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, light);
              __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, light);
#else
              tim_compare = light;
#endif
          }
          light++;
          if(light >= 200){
              light = 0;
          }
      }
      osDelay(5);
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
    uint32_t beep_tone = 99;
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
      else if (HAL_GPIO_ReadPin(KEY6_GPIO_Port,KEY6_Pin)==GPIO_PIN_SET){
          osDelay(10);
          if(HAL_GPIO_ReadPin(KEY6_GPIO_Port,KEY6_Pin)==GPIO_PIN_SET)
          {
              key_state = 6;
          }
          while(HAL_GPIO_ReadPin(KEY6_GPIO_Port,KEY6_Pin)==GPIO_PIN_SET)
          {
              osDelay(1);
          }
      }
      //按键触发控制
      switch (key_state) {
          case 1:
              beep_tone += 10;
              printf("%lu\r\n", beep_tone);
              __HAL_TIM_SET_AUTORELOAD(&htim4, beep_tone);
              break;

          case 2:
              beep_switch = 2;
              break;

          case 3:
              beep_switch = 1;
              break;

          case 4:
              if (beep_tone > 0){
                  beep_tone -= 10;
              }
              printf("%lu\r\n", beep_tone);
              __HAL_TIM_SET_AUTORELOAD(&htim4, beep_tone);
              break;

          case 5:
              if (led_breathe == true){
                  led_breathe = false;
              }
              else{
                  led_breathe = true;
              }
              break;

          case 6:
              if(led_breathe == false){
                  if(light < 80){
                      light += 20;
                  }
                  else{
                      light = 0;
                  }
#if led_use_pwm
                  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, light);
                  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, light);
                  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, light);
                  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, light);
#else
                  tim_compare = light;
#endif
              }
              break;

          default:
              break;
      }

      osDelay(1);
  }
  /* USER CODE END StartKeyTask */
}

/* USER CODE BEGIN Header_StartBeepTask */
/**
* @brief Function implementing the BeepTask thread.
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
      if(beep_switch == 1){//启动
          HAL_TIM_Base_Start_IT(&htim4);
          beep_switch = 0;
      }
      else if(beep_switch == 2){//停止
          HAL_TIM_Base_Stop_IT(&htim4);
          beep_switch = 0;
      }
      else if(beep_switch == 3){//叫一声
          HAL_TIM_Base_Start_IT(&htim4);
          osDelay(100);
          HAL_TIM_Base_Stop_IT(&htim4);
          beep_switch = 0;
      }
      osDelay(1);
  }
  /* USER CODE END StartBeepTask */
}

/* USER CODE BEGIN Header_StartCaptureTask */
/**
* @brief Function implementing the CaptureTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCaptureTask */
void StartCaptureTask(void *argument)
{
  /* USER CODE BEGIN StartCaptureTask */
  uint32_t tick = 0;

  HAL_TIM_Base_Start_IT(&htim2);
  /* Infinite loop */
  for(;;)
  {
      if(osKernelGetTickCount() >= tick){
          tick = osKernelGetTickCount() + 10;

          switch (capture_cnt) {
              case 0:
                  capture_cnt++;
                  __HAL_TIM_SET_CAPTUREPOLARITY(&htim9, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);//设置上升沿触发
                  HAL_TIM_Base_Start_IT(&htim9);//启动定时器并打开中断
                  HAL_TIM_IC_Start_IT(&htim9, TIM_CHANNEL_2);//启动输入捕获
                  break;

              case 3:
                  high_time = capture_buf[2] * 65536 + capture_buf[1] - capture_buf[0];
                  printf("K6 press time:%.1fms\r\n", high_time / 10.0f);
                  if (high_time > 200){
                      beep_switch = 3;
                      key6_cnt++;
                  }
                  capture_cnt = 0;
                  tick = osKernelGetTickCount() + 100;
                  break;

              default:
                  break;
          }
      }
      osDelay(1);
  }
  /* USER CODE END StartCaptureTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
