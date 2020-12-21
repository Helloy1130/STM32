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
#include <string.h>
#include <stdbool.h>
#include "tim.h"
#include "GUI.h"
#include "usart.h"
#include "DS_18B20.h"
#include "adc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern GUI_CONST_STORAGE GUI_BITMAP bmCN_bilibiliB;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_12;
extern uint32_t recv_len;//接收数据长度
extern uint8_t dma_buf[MAX_BUF_LEN];//DMA接收缓存
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint8_t key_state = 0;
uint8_t key_buf = 0;
bool beep_switch = 0;

float temp;
int temp_max = 40;
uint16_t val[16];//DMA采样数据缓存
uint16_t adc_val1 = 0;//通道1采样数据
uint16_t adc_val2 = 0;//通道2采样数据
bool touch_button = false;
bool touch_flag = true;
/* USER CODE END Variables */
/* Definitions for CollectTask */
osThreadId_t CollectTaskHandle;
const osThreadAttr_t CollectTask_attributes = {
  .name = "CollectTask",
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
/* Definitions for GUITask */
osThreadId_t GUITaskHandle;
const osThreadAttr_t GUITask_attributes = {
  .name = "GUITask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 256 * 4
};
/* Definitions for UartTask */
osThreadId_t UartTaskHandle;
const osThreadAttr_t UartTask_attributes = {
  .name = "UartTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartCollectTask(void *argument);
void StartKeyTask(void *argument);
void StartBeepTask(void *argument);
void StartGUITask(void *argument);
void StartUartTask(void *argument);

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
  /* creation of CollectTask */
  CollectTaskHandle = osThreadNew(StartCollectTask, NULL, &CollectTask_attributes);

  /* creation of keyTask */
  keyTaskHandle = osThreadNew(StartKeyTask, NULL, &keyTask_attributes);

  /* creation of beepTask */
  beepTaskHandle = osThreadNew(StartBeepTask, NULL, &beepTask_attributes);

  /* creation of GUITask */
  GUITaskHandle = osThreadNew(StartGUITask, NULL, &GUITask_attributes);

  /* creation of UartTask */
  UartTaskHandle = osThreadNew(StartUartTask, NULL, &UartTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartCollectTask */
/**
  * @brief  Function implementing the CollectTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartCollectTask */
void StartCollectTask(void *argument)
{
  /* USER CODE BEGIN StartCollectTask */
    int touch_sum = 0;
    int touch_count = 0;

    ds18b20_init();
    uint32_t tick = osKernelGetTickCount() + 100;
    uint32_t tick2 = osKernelGetTickCount() + 10;
#if (ADC_MODE == 2)
    HAL_ADC_Start_IT(&hadc1);
#endif
#if (ADC_MODE == 3)
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)val, 16);
#endif
  /* Infinite loop */
  for(;;)
  {
      if (osKernelGetTickCount() >= tick){//100ms 读取一次
          tick = osKernelGetTickCount() + 100;
          temp = ds18b20_read();
#if (ADC_MODE == 1)
          int adc_val1 = 0; int adc_val2 = 0;
          HAL_ADC_Start(&hadc1); //查询方式AD采集，每次采集两个通道
          if(HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK){//等待第1个采集额完成
              adc_val1 = HAL_ADC_GetValue(&hadc1);
          }
          if(HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK){//等待第2个采集额完成
              adc_val2 = HAL_ADC_GetValue(&hadc1);
          }
          printf("temperature = %5.1f, adc_val1 = %5d, adc_val2 = %5d\r\n", temp, adc_val1, adc_val2);
#endif
#if (ADC_MODE == 2)
          printf("temperature = %5.1f, adc_val1 = %5d, adc_val2 = %5d\r\n", temp, adc_val1, adc_val2);
          if (HAL_ADC_GetValue(&hadc1) & HAL_ADC_STATE_REG_EOC){
              HAL_ADC_Start_IT(&hadc1);
          }

#endif
#if (ADC_MODE == 3)
//          printf("temperature = %5.1f C, adc_val1 = %5d, adc_val2 = %5d\r\n", temp, adc_val1, adc_val2);
#endif
      }
      if (osKernelGetTickCount() >= tick2 && touch_flag == true) {//10ms 快速采集
          tick2 = osKernelGetTickCount() + 10;
          touch_sum += adc_val2;
          touch_count++;
          if (touch_count > 10){
              if (touch_sum / touch_count  * 3.3 / 4095 < 2.98){
                  touch_button = true;
                  touch_flag = false;
              }
              else{
                  touch_button = false;
              }
              touch_sum = 0;
              touch_count = 0;
          }
      }
      osDelay(1);
  }
  /* USER CODE END StartCollectTask */
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
              key_buf = 1;
              break;

          case 2:
              key_buf = 2;
              break;

          case 3:
              key_buf = 3;
              break;

          case 4:
              key_buf = 4;
              break;

          case 5:
              key_buf = 5;
              break;

          case 6:
              key_buf = 6;
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
* @brief Function implementing the beepTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBeepTask */
void StartBeepTask(void *argument)
{
  /* USER CODE BEGIN StartBeepTask */
    uint32_t tick = osKernelGetTickCount() + 5000;
  /* Infinite loop */
  for(;;)
  {
      if (beep_switch == true) {
          if (osKernelGetTickCount() >= tick) {//5s 报警一次嘀嘀嘀
              tick = osKernelGetTickCount() + 5000;
              for (int i = 0; i < 3; ++i) {
                  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);//使能输出
                  osDelay(100);
                  HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
                  osDelay(100);
              }
              printf("The current temperature is too high\r\n");
          }
      }

      osDelay(1);
  }
  /* USER CODE END StartBeepTask */
}

/* USER CODE BEGIN Header_StartGUITask */
/**
* @brief Function implementing the GUITask thread.
* @param argument: Not used
* @retval None
*/
int temp_data_num = 0;
int temp_data[125] = {0};
int adc_data1_num = 0;
int adc_data1[125] = {0};
int adc_data2_num = 0;
int adc_data2[125] = {0};
/* USER CODE END Header_StartGUITask */
void StartGUITask(void *argument)
{
  /* USER CODE BEGIN StartGUITask */
    uint8_t menu;
    uint8_t choose_menu;
    char show_data[30];

    GUI_Init();
    GUI_DrawBitmap(&bmCN_bilibiliB, 0, 0);
    GUI_Update();//刷新显示
    osDelay(2000);
    GUI_Clear();

    GUI_SetFont(&GUI_FontHZ_SimSun_12);
    GUI_DispString("学号姓名界面\n");
    GUI_DispStringAt("学号：18042209\n", 0, 20);
    GUI_DispString("姓名：陈俊煜\n");
    GUI_DispString("课程：嵌入式系统实验\n");
    GUI_Update();//刷新显示
    osDelay(2000);
    menu = 0;//进入主菜单
    choose_menu = 1;
    uint32_t tick = osKernelGetTickCount() + 100;

    /* Infinite loop */
  for(;;)
  {
      if (menu == 0) {//主菜单
          GUI_Clear();
          GUI_DispStringAt("主菜单", 0, 0);
          GUI_DispStringAt("温度与ADC电压显示", 15, 16);
          GUI_DispStringAt("温度数据曲线", 15, 28);
          GUI_DispStringAt("麦克风数据曲线", 15, 40);
          GUI_DispStringAt("触摸按键数据曲线", 15, 52);

          switch (key_buf) {
              case 1://上
                  if(choose_menu > 1){
                      choose_menu--;
                  }
                  else{
                      choose_menu = 4;
                  }
                  key_buf = 0;
                  break;

              case 2://下
                  if(choose_menu < 4){
                      choose_menu++;
                  }
                  else{
                      choose_menu = 1;
                  }
                  key_buf = 0;
                  break;

              case 3://进入功能
                  menu = choose_menu;
                  key_buf = 0;
                  break;

              default:
                  break;
          }

          GUI_DispStringAt("->", 0, 16 + 12 * (choose_menu - 1));
          GUI_Update();//刷新显示
      }
      if (menu == 1){//温度与ADC电压显示界面
          if (osKernelGetTickCount() >= tick) {//100ms 显示一次
              tick = osKernelGetTickCount() + 100;
              GUI_Clear();
              GUI_DispStringAt("温度与ADC电压显示", 0, 0);
              sprintf(show_data, "温度：%5.1f ℃", temp);
              GUI_DispStringAt(show_data, 0, 16);
              sprintf(show_data, "麦克风电压：%5.1f V", adc_val1 * 3.3 / 4095);
              GUI_DispStringAt(show_data, 0, 28);
              sprintf(show_data, "触摸按键电压：%5.2f V", adc_val2 * 3.3 / 4095);
              GUI_DispStringAt(show_data, 0, 40);
              sprintf(show_data, "<温度上限>：%3d ℃", temp_max);
              GUI_DispStringAt(show_data, 0, 52);

              switch (key_buf) {
                  case 1://增加温度上限
                      temp_max++;
                      key_buf = 0;
                      break;

                  case 2://减少温度上限
                      temp_max--;
                      key_buf = 0;
                      break;

                  case 4://退出功能
                      menu = 0;
                      key_buf = 0;
                      break;

                  default:
                      break;
              }
              if (temp > temp_max){
                  beep_switch = true;
              }
              else{
                  beep_switch = false;
              }
              GUI_Update();//刷新显示
          }
      }
      if (menu == 2){//温度数据曲线
          if (osKernelGetTickCount() >= tick) {//100ms 显示一次
              tick = osKernelGetTickCount() + 100;
              GUI_Clear();
              GUI_DispStringAt("温度曲线", 0, 0);
              sprintf(show_data, "%5.1f ℃", temp);
              GUI_DispStringAt(show_data, 80, 0);
              GUI_DrawRect(0, 16, 126, 63);
              int y = 62 - (temp - 15) * 1.5;// 15-45
              if (temp_data_num < 125) {
                  temp_data[temp_data_num++] = y;//0-124
              } else {
                  for (int i = 0; i < 124; ++i) {
                      temp_data[i] = temp_data[i + 1];
                  }
                  temp_data[124] = y;
              }
              for (int i = 0; i < temp_data_num - 1; ++i) {
                  GUI_DrawLine(i, temp_data[i], i + 1, temp_data[i + 1]);
              }
              GUI_Update();//刷新显示

              switch (key_buf) {
                  case 4://退出功能
                      menu = 0;
                      key_buf = 0;
                      break;

                  default:
                      break;
              }
          }
      }
      if (menu == 3){//麦克风数据曲线
          if (osKernelGetTickCount() >= tick) {//100ms 显示一次
              tick = osKernelGetTickCount() + 100;
              GUI_Clear();
              GUI_DispStringAt("麦克风曲线", 0, 0);
              sprintf(show_data, "%5.1f V", adc_val1 * 3.3 / 4095);
              GUI_DispStringAt(show_data, 83, 0);
              GUI_DrawRect(0, 16, 126, 63);
              int y = 62 - (adc_val1 * 3.3 / 4095 - 0) * 13.64;// 0-3.3
              if (adc_data1_num < 125) {
                  adc_data1[adc_data1_num++] = y;//0-124
              }
              else {
                  for (int i = 0; i < 124; ++i) {
                      adc_data1[i] = adc_data1[i + 1];
                  }
                  adc_data1[124] = y;
              }
              for (int i = 0; i < adc_data1_num - 1; ++i) {
                  GUI_DrawLine(i, adc_data1[i], i + 1, adc_data1[i + 1]);
              }
              GUI_Update();//刷新显示

              switch (key_buf) {
                  case 4://退出功能
                      menu = 0;
                      key_buf = 0;
                      break;

                  default:
                      break;
              }
          }
      }
      if (menu == 4){//触摸按键数据曲线
          if (osKernelGetTickCount() >= tick) {//100ms 显示一次
              tick = osKernelGetTickCount() + 100;
              GUI_Clear();
              GUI_DispStringAt("触摸按键曲线", 0, 0);
              sprintf(show_data, "%5.2f V", adc_val2 * 3.3 / 4095);
              GUI_DispStringAt(show_data, 83, 0);
              GUI_DrawRect(0, 16, 126, 63);
              int y = 62 - (adc_val2 * 3.3 / 4095 - 2.7) * 90;// 2.7-3.2
              if (adc_data2_num < 125) {
                  adc_data2[adc_data2_num++] = y;//0-124
              }
              else {
                  for (int i = 0; i < 124; ++i) {
                      adc_data2[i] = adc_data2[i + 1];
                  }
                  adc_data2[124] = y;
              }
              for (int i = 0; i < adc_data2_num - 1; ++i) {
                  GUI_DrawLine(i, adc_data2[i], i + 1, adc_data2[i + 1]);
              }
              if (touch_button){
                  GUI_FillRect(114, 16, 126, 28);
                  touch_flag = true;
              }
              GUI_Update();//刷新显示

              switch (key_buf) {
                  case 4://退出功能
                      menu = 0;
                      key_buf = 0;
                      break;

                  default:
                      break;
              }
          }
      }
      osDelay(1);
  }
  /* USER CODE END StartGUITask */
}

/* USER CODE BEGIN Header_StartUartTask */
/**
* @brief Function implementing the UartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartTask */
void StartUartTask(void *argument)
{
  /* USER CODE BEGIN StartUartTask */
    int8_t sign_flag;
    int recv_temp;
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);//使能idle中断
    HAL_UART_Receive_DMA(&huart1, dma_buf, MAX_BUF_LEN);
  /* Infinite loop */
  for(;;)
  {
      if(recv_len > 0) {
//          printf("%s", dma_buf);
          if (strstr((char *)dma_buf, "Set Temp Max = ") != NULL){
              recv_temp = 0;
              sign_flag = 1;
              for (int i = 0; i < recv_len - 15 - 2; ++i) {
                  if (i == 0 && dma_buf[15 + i] == '-'){//考虑负数
                      sign_flag = -1;
                  }
                  else if (dma_buf[15 + i] >= '0' && dma_buf[15 + i] <= '9'){
                      recv_temp = recv_temp * 10 + (dma_buf[15 + i] - '0');
                      if (i == recv_len - 18){
                          temp_max = recv_temp * sign_flag;
                      }
                  }
                  else {
                      printf("Please check the input! (Example:Set Temp Max = 40)\r\n");
                      break;
                  }
              }
          }
          else {
              printf("Please check the input! (Example:Set Temp Max = 40)\r\n");
          }
          recv_len = 0;
      }
      osDelay(1);
  }
  /* USER CODE END StartUartTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
