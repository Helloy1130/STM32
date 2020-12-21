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
#include <string.h>
#include "tim.h"
#include "LED.h"
#include "DigitalTube.h"
#include "GUI.h"
#include "usart.h"
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
uint8_t led_id = 1;
bool led_switch = true;
uint32_t led_control_time = 200;
uint8_t key_state = 0;
uint8_t key_buf = 0;
uint8_t beep_switch = 0;
int beep_time;
char data_buf[13][21];//要显示的数据
uint8_t data_page_max = 0;
uint8_t data_page = 0;
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

void StartLedTask(void *argument);
void StartKeyTask(void *argument);
void StartBeepTask(void *argument);
void StartDigitaTubeTask(void *argument);
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
  /* creation of ledTask */
  ledTaskHandle = osThreadNew(StartLedTask, NULL, &ledTask_attributes);

  /* creation of keyTask */
  keyTaskHandle = osThreadNew(StartKeyTask, NULL, &keyTask_attributes);

  /* creation of beepTask */
  beepTaskHandle = osThreadNew(StartBeepTask, NULL, &beepTask_attributes);

  /* creation of digitaTubeTask */
  digitaTubeTaskHandle = osThreadNew(StartDigitaTubeTask, NULL, &digitaTubeTask_attributes);

  /* creation of GUITask */
  GUITaskHandle = osThreadNew(StartGUITask, NULL, &GUITask_attributes);

  /* creation of UartTask */
  UartTaskHandle = osThreadNew(StartUartTask, NULL, &UartTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
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
  uint32_t beep_time_tick = 0;
  /* Infinite loop */
  for(;;)
  {
      if (beep_switch == 1) {
          HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);//使能输出
          beep_time_tick = osKernelGetTickCount();
          beep_switch = 0;
      }
      else if (beep_switch == 2){
          HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);//失能输出
          beep_time = osKernelGetTickCount() - beep_time_tick;
          beep_time_tick = 0;
          beep_switch = 0;
      }

      osDelay(1);
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

/* USER CODE BEGIN Header_StartGUITask */
/**
* @brief Function implementing the GUITask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGUITask */
void StartGUITask(void *argument)
{
  /* USER CODE BEGIN StartGUITask */
    uint8_t menu;
    uint16_t beep_tune = 300;
    char beep_time_show[20];

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
    menu = 2;//进入主菜单
  /* Infinite loop */
  for(;;)
  {
      if (menu == 1) {//学号姓名显示
          GUI_Clear();
          GUI_DispStringAt("学号姓名界面\n", 0, 0);
          GUI_DispStringAt("学号：18042209\n", 0, 20);
          GUI_DispString("姓名：陈俊煜\n");
          GUI_DispString("课程：嵌入式系统实验\n");
          GUI_Update();//刷新显示
          osDelay(2000);
          menu = 2;//进入主菜单
      }
      if (menu == 2) {//主菜单下
          GUI_Clear();
          GUI_DispStringAt("主菜单\n", 0, 0);
          GUI_DispStringAt("SW1:流水灯测试\n", 0, 16);
          GUI_DispString("SW2:按键测试\n");
          GUI_DispString("SW3:蜂鸣器测试\n");
          GUI_DispString("SW4:串口数据显示\n");
          GUI_Update();//刷新显示

          switch (key_buf) {
              case 1://流水灯测试
                  menu = 3;
                  key_buf = 0;
                  break;

              case 2://按键测试
                  menu = 4;
                  key_buf = 0;
                  break;

              case 3://蜂鸣器测试
                  menu = 5;
                  key_buf = 0;
                  break;

              case 4://串口数据显示功能
                  menu = 6;
                  key_buf = 0;
                  data_page = 1;
                  break;

              default:
                  break;
          }
      }
      if (menu == 3) {//流水灯测试功能
          GUI_Clear();
          GUI_DispStringAt("流水灯测试\n", 0, 0);
          GUI_DispStringAt("SW5:返回学号姓名界面\n", 0, 40);
          GUI_DispString("SW6:返回主菜单\n");
          GUI_DispStringAt("LED:\n", 0, 20);
          GUI_FillRect(30 + (led_id - 1) * 8, 23, 30 + led_id * 8, 30);
          GUI_Update();//刷新显示

          switch (key_buf) {
              case 5://返回学号姓名界面
                  menu = 1;
                  key_buf = 0;
                  break;

              case 6://返回主菜单
                  menu = 2;
                  key_buf = 0;
                  break;

              default:
                  break;
          }
      }
      if (menu == 4) {//按键测试功能
          GUI_Clear();
          GUI_DispStringAt("按键测试\n", 0, 0);
          GUI_DispStringAt("SW5:返回学号姓名界面\n", 0, 40);
          GUI_DispString("SW6:返回主菜单\n");
          GUI_DispStringAt("KEY: 1 2 3 4 5 6\n", 0, 15);
          if(key_buf != 0){
              GUI_FillCircle(32 + 12 * (key_buf - 1), 31, 3);
          }
          GUI_Update();//刷新显示

          switch (key_buf) {
              case 5://返回学号姓名界面
                  menu = 1;
                  key_buf = 0;
                  break;

              case 6://返回主菜单
                  menu = 2;
                  key_buf = 0;
                  break;

              default:
                  break;
          }
      }
      if (menu == 5) {//蜂鸣器测试功能
          GUI_Clear();
          GUI_DispStringAt("蜂鸣器测试\n", 0, 0);
          GUI_DispStringAt("SW5:返回学号姓名界面\n", 0, 40);
          GUI_DispString("SW6:返回主菜单\n");
          GUI_DispStringAt("BEEP Time:", 0, 15);
          sprintf(beep_time_show, "%d", beep_time);
          GUI_DispStringAt(beep_time_show, 65, 15);
          GUI_DispStringAt("ms\n", 100, 15);
          GUI_Update();//刷新显示

          switch (key_buf) {
              case 1://启动蜂鸣器
                  beep_switch = 1;
                  key_buf = 0;
                  break;

              case 2://停止蜂鸣器
                  beep_switch = 2;
                  key_buf = 0;
                  break;

              case 3://调节音调 变低沉
                  if (beep_tune < 600) {
                      beep_tune += 10;
                  }
                  __HAL_TIM_SET_AUTORELOAD(&htim3, beep_tune);
                  key_buf = 0;
                  break;

              case 4://调节音调 变尖锐
                  if (beep_tune > 250) {
                      beep_tune -= 10;
                  }
                  __HAL_TIM_SET_AUTORELOAD(&htim3, beep_tune);
                  key_buf = 0;
                  break;

              case 5://返回学号姓名界面
                  menu = 1;
                  key_buf = 0;
                  break;

              case 6://返回主菜单
                  menu = 2;
                  key_buf = 0;
                  break;

              default:
                  break;
          }
      }
      if (menu == 6) {//串口数据显示功能
          GUI_Clear();
          GUI_DispStringAt("串口数据显示\n", 0, 0);
          GUI_DispStringAt("SW5:返回学号姓名界面\n", 0, 40);
          GUI_DispString("SW6:返回主菜单\n");
          char show_data[10] = {0};
          sprintf(show_data, "%d / %d", data_page, data_page_max/2);
          GUI_DispStringAt(show_data, 95, 0);
          GUI_DispStringAt(data_buf[data_page * 2 - 2], 0, 15);
          GUI_DispStringAt(data_buf[data_page * 2 - 1], 0, 25);
          GUI_Update();//刷新显示

          switch (key_buf) {
              case 1:
                  if (data_page > 1){
                      data_page--;
                  }
                  key_buf = 0;
                  break;

              case 2:
                  if(data_page < data_page_max / 2 + data_page_max % 2){
                      data_page++;
                  }
                  key_buf = 0;
                  break;

              case 5://返回学号姓名界面
                  menu = 1;
                  key_buf = 0;
                  break;

              case 6://返回主菜单
                  menu = 2;
                  key_buf = 0;
                  break;

              default:
                  break;
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
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);//使能idle中断
    HAL_UART_Receive_DMA(&huart1, dma_buf, MAX_BUF_LEN);
  /* Infinite loop */
  for(;;)
  {
      if(recv_len > 0) {
//          printf("%s", dma_buf);
          data_page_max = 0;
          uint8_t j = 0;
          for (int i = 0; i < recv_len; ++i) {
              data_buf[data_page_max][j] = dma_buf[i];
              j++;
              if(dma_buf[i] == '\n'){
                  data_page_max++;
                  j = 0;
              }
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
