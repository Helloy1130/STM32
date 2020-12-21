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
#include "LED.h"
#include "DigitalTube.h"
#include "usart.h"
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
bool led_switch = false;
uint32_t led_control_time = 200;
bool beep_switch = false;
uint8_t show_data[4];

#if (UART_Receive_useMsg == 0)
extern uint8_t rx1_buf[MAX_RECV_LEN];
extern uint8_t line_flag;
#endif
extern uint8_t *pBuf;
#if UART_Receive_useDMA
extern uint32_t recv_len;//接收数据长度
extern uint8_t dma_buf[MAX_BUF_LEN];//DMA接收缓存
#endif

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
/* Definitions for digitaTubeTask */
osThreadId_t digitaTubeTaskHandle;
const osThreadAttr_t digitaTubeTask_attributes = {
  .name = "digitaTubeTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for uartReceiveTask */
osThreadId_t uartReceiveTaskHandle;
const osThreadAttr_t uartReceiveTask_attributes = {
  .name = "uartReceiveTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for uartProcessTask */
osThreadId_t uartProcessTaskHandle;
const osThreadAttr_t uartProcessTask_attributes = {
  .name = "uartProcessTask",
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
/* Definitions for temporaryTask */
osThreadId_t temporaryTaskHandle;
const osThreadAttr_t temporaryTask_attributes = {
  .name = "temporaryTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for uartDMATask */
osThreadId_t uartDMATaskHandle;
const osThreadAttr_t uartDMATask_attributes = {
  .name = "uartDMATask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for uartQueue */
osMessageQueueId_t uartQueueHandle;
const osMessageQueueAttr_t uartQueue_attributes = {
  .name = "uartQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartLedTask(void *argument);
void StartKeyTask(void *argument);
void StartDigitaTubeTask(void *argument);
void StartUartReceiveTask(void *argument);
void StartUartProcessTask(void *argument);
void StartBeepTask(void *argument);
void StartTemporaryTask(void *argument);
void StartUartDMATask(void *argument);

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

  /* Create the queue(s) */
  /* creation of uartQueue */
  uartQueueHandle = osMessageQueueNew (16, 128, &uartQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ledTask */
  ledTaskHandle = osThreadNew(StartLedTask, NULL, &ledTask_attributes);

  /* creation of keyTask */
  keyTaskHandle = osThreadNew(StartKeyTask, NULL, &keyTask_attributes);

  /* creation of digitaTubeTask */
  digitaTubeTaskHandle = osThreadNew(StartDigitaTubeTask, NULL, &digitaTubeTask_attributes);

  /* creation of uartReceiveTask */
  uartReceiveTaskHandle = osThreadNew(StartUartReceiveTask, NULL, &uartReceiveTask_attributes);

  /* creation of uartProcessTask */
  uartProcessTaskHandle = osThreadNew(StartUartProcessTask, NULL, &uartProcessTask_attributes);

  /* creation of beepTask */
  beepTaskHandle = osThreadNew(StartBeepTask, NULL, &beepTask_attributes);

  /* creation of temporaryTask */
  temporaryTaskHandle = osThreadNew(StartTemporaryTask, NULL, &temporaryTask_attributes);

  /* creation of uartDMATask */
  uartDMATaskHandle = osThreadNew(StartUartDMATask, NULL, &uartDMATask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
    osThreadSuspend(temporaryTaskHandle);//挂起临时任务
    osThreadTerminate(temporaryTaskHandle);//删除临时任务

#if (UART_Receive_useNormal == 0)
    osThreadSuspend(uartReceiveTaskHandle);//挂起
    osThreadTerminate(uartReceiveTaskHandle);//删除串口轮询方式任务
#endif

#if (UART_Receive_useIT == 0)
    osThreadSuspend(uartProcessTaskHandle);//挂起
    osThreadTerminate(uartProcessTaskHandle);//删除串口中断接收方式处理任务
#endif

#if (UART_Receive_useDMA == 0)
    osThreadSuspend(uartDMATaskHandle);//挂起
    osThreadTerminate(uartDMATaskHandle);//删除串口DMA接收方式处理任务
#endif

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
    uint8_t led_id = 1;
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
    uint8_t key_state;
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
              beep_switch = true;
              show_data[0] = 1; show_data[1] = 0; show_data[2] = 0; show_data[3] = 0;
              printf("led_switch = %d,led_control_time = %d\r\n",led_switch,(int)led_control_time);
              break;

          case 2://减速流水灯
              if(led_control_time<400)led_control_time+=10;
              else led_control_time = 400;
              __HAL_TIM_SET_AUTORELOAD(&htim3,399);
              beep_switch = true;
              show_data[0] = 2; show_data[1] = 0; show_data[2] = 0; show_data[3] = 0;
              printf("led_switch = %d,led_control_time = %d\r\n",led_switch,(int)led_control_time);
              break;

          case 3://加速流水灯
              if(led_control_time>10) led_control_time-=10;
              else led_control_time = 10;
              __HAL_TIM_SET_AUTORELOAD(&htim3,499);
              beep_switch = true;
              show_data[0] = 3; show_data[1] = 0; show_data[2] = 0; show_data[3] = 0;
              printf("led_switch = %d,led_control_time = %d\r\n",led_switch,(int)led_control_time);
              break;

          case 4://暂停流水灯
              led_switch=false;
              __HAL_TIM_SET_AUTORELOAD(&htim3,599);
              beep_switch = true;
              show_data[0] = 4; show_data[1] = 0; show_data[2] = 0; show_data[3] = 0;
              printf("led_switch = %d,led_control_time = %d\r\n",led_switch,(int)led_control_time);
              break;

          case 5:
              osThreadSuspend(digitaTubeTaskHandle);//挂起数码管显示任务
              temporaryTaskHandle = osThreadNew(StartTemporaryTask, NULL, &temporaryTask_attributes);//新建临时任务
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
      for (int i = 0; i < 4; ++i) {
          Write595(i,show_data[i],0);
          osDelay(5);
      }
  }
  /* USER CODE END StartDigitaTubeTask */
}

/* USER CODE BEGIN Header_StartUartReceiveTask */
/**
* @brief Function implementing the uartReceiveTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartReceiveTask */
void StartUartReceiveTask(void *argument)
{
  /* USER CODE BEGIN StartUartReceiveTask */
  uint8_t rx_data;
  /* Infinite loop */
  for(;;)
  {
      if (HAL_UART_Receive(&huart1, &rx_data, 1, 100) == HAL_OK){
          printf("%c", rx_data);
      }
#if (UART_Receive_useNormal==0)
      osDelay(1);//阻塞方式读取数据，不需要延时
#endif
  }
  /* USER CODE END StartUartReceiveTask */
}

/* USER CODE BEGIN Header_StartUartProcessTask */
/**
* @brief Function implementing the uartProcessTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartProcessTask */
void StartUartProcessTask(void *argument)
{
  /* USER CODE BEGIN StartUartProcessTask */
#if UART_Receive_useIT
    HAL_UART_Receive_IT(&huart1, pBuf, 1);
#endif
#if UART_Receive_useMsg
    uint8_t rx1_buf[MAX_RECV_LEN];
#endif
  /* Infinite loop */
  for(;;)
  {
#if UART_Receive_useMsg
      if (osMessageQueueGet(uartQueueHandle, rx1_buf, 0, 0) == osOK){
#else
      if (line_flag) {
#endif
          if(strcmp((char *)rx1_buf, "START\r\n") == 0) {
              led_switch = true;
              printf("OK\r\n");
          }
          else if(strcmp((char *)rx1_buf, "STOP\r\n") == 0) {
              led_switch = false;
              printf("OK\r\n");
          }
          else if(strcmp((char *)rx1_buf, "SPEED") > 0 && strlen((char *)rx1_buf) == 8) {
              if (rx1_buf[5] >= '1' && rx1_buf[5] <= '9') {
                  led_control_time = 200 - (rx1_buf[5] - '0') * 20;
                  printf("OK\r\n");
              }
              else{
                  printf("%s", rx1_buf);
              }
          }
          else if((rx1_buf[0] >= '0' && rx1_buf[0] <= '9') | (rx1_buf[0] >= 'A' && rx1_buf[0] <= 'F')) {
              switch (strlen((char  *)rx1_buf)) {
                  case 3:
                      if(rx1_buf[0] >= '0' && rx1_buf[0] <= '9'){
                          show_data[0] = rx1_buf[0] - '0';
                      }
                      else if (rx1_buf[0] >= 'A' && rx1_buf[0] <= 'F'){
                          show_data[0] = rx1_buf[0] - 'A' + 10;
                      }
                      show_data[1] = 0; show_data[2] = 0; show_data[3] = 0;
                      printf("OK\r\n");
                      break;

                  case 4:
                      if((rx1_buf[1] >= '0' && rx1_buf[1] <= '9') | (rx1_buf[1] >= 'A' && rx1_buf[1] <= 'F')){
                          for (int i = 0; i < 2; ++i) {
                              if(rx1_buf[i] >= '0' && rx1_buf[i] <= '9'){
                                  show_data[i] = rx1_buf[i] - '0';
                              }
                              else if (rx1_buf[i] >= 'A' && rx1_buf[i] <= 'F'){
                                  show_data[i] = rx1_buf[i] - 'A' + 10;
                              }
                          }
                          show_data[2] = 0; show_data[3] = 0;
                          printf("OK\r\n");
                      }
                      else{
                          printf("%s", rx1_buf);
                      }
                      break;

                  case 5:
                      if(((rx1_buf[1] >= '0' && rx1_buf[1] <= '9') | (rx1_buf[1] >= 'A' && rx1_buf[1] <= 'F')) &&
                         ((rx1_buf[2] >= '0' && rx1_buf[2] <= '9') | (rx1_buf[2] >= 'A' && rx1_buf[2] <= 'F'))){
                          for (int i = 0; i < 3; ++i) {
                              if(rx1_buf[i] >= '0' && rx1_buf[i] <= '9'){
                                  show_data[i] = rx1_buf[i] - '0';
                              }
                              else if (rx1_buf[i] >= 'A' && rx1_buf[i] <= 'F'){
                                  show_data[i] = rx1_buf[i] - 'A' + 10;
                              }
                          }
                          show_data[3] = 0;
                          printf("OK\r\n");
                      }
                      else{
                          printf("%s", rx1_buf);
                      }
                      break;

                  case 6:
                      if(((rx1_buf[1] >= '0' && rx1_buf[1] <= '9') | (rx1_buf[1] >= 'A' && rx1_buf[1] <= 'F')) &&
                         ((rx1_buf[2] >= '0' && rx1_buf[2] <= '9') | (rx1_buf[2] >= 'A' && rx1_buf[2] <= 'F')) &&
                         ((rx1_buf[3] >= '0' && rx1_buf[3] <= '9') | (rx1_buf[3] >= 'A' && rx1_buf[3] <= 'F'))){
                          for (int i = 0; i < 4; ++i) {
                              if(rx1_buf[i] >= '0' && rx1_buf[i] <= '9'){
                                  show_data[i] = rx1_buf[i] - '0';
                              }
                              else if (rx1_buf[i] >= 'A' && rx1_buf[i] <= 'F'){
                                  show_data[i] = rx1_buf[i] - 'A' + 10;
                              }
                          }
                          printf("OK\r\n");
                      }
                      else{
                          printf("%s", rx1_buf);
                      }
                      break;

                  default:
                      printf("%s", rx1_buf);
                      break;
              }
          }
          else{
              printf("%s", rx1_buf);
          }
#if (UART_Receive_useMsg == 0)
          line_flag = 0;
#endif
      }

      osDelay(1);
  }
  /* USER CODE END StartUartProcessTask */
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

/* USER CODE BEGIN Header_StartUartDMATask */
/**
* @brief Function implementing the uartDMATask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartDMATask */
void StartUartDMATask(void *argument)
{
  /* USER CODE BEGIN StartUartDMATask */
#if UART_Receive_useDMA
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);//使能idle中断
    HAL_UART_Receive_DMA(&huart1, dma_buf, MAX_BUF_LEN);
#endif
  /* Infinite loop */
  for(;;)
  {
#if UART_Receive_useDMA
      if(recv_len > 0) {
          if(strcmp((char *)dma_buf, "START\r\n") == 0) {
              led_switch = true;
              printf("OK\r\n");
          }
          else if(strcmp((char *)dma_buf, "STOP\r\n") == 0) {
              led_switch = false;
              printf("OK\r\n");
          }
          else if(strcmp((char *)dma_buf, "SPEED") > 0 && strlen((char *)dma_buf) == 8) {
              if (dma_buf[5] >= '1' && dma_buf[5] <= '9') {
                  led_control_time = 200 - (dma_buf[5] - '0') * 20;
                  printf("OK\r\n");
              }
              else{
                  printf("%s", dma_buf);
              }
          }
          else if((dma_buf[0] >= '0' && dma_buf[0] <= '9') | (dma_buf[0] >= 'A' && dma_buf[0] <= 'F')) {
              switch (strlen((char  *)dma_buf)) {
                  case 3:
                      if(dma_buf[0] >= '0' && dma_buf[0] <= '9'){
                          show_data[0] = dma_buf[0] - '0';
                      }
                      else if (dma_buf[0] >= 'A' && dma_buf[0] <= 'F'){
                          show_data[0] = dma_buf[0] - 'A' + 10;
                      }
                      show_data[1] = 0; show_data[2] = 0; show_data[3] = 0;
                      printf("OK\r\n");
                      break;

                  case 4:
                      if((dma_buf[1] >= '0' && dma_buf[1] <= '9') | (dma_buf[1] >= 'A' && dma_buf[1] <= 'F')){
                          for (int i = 0; i < 2; ++i) {
                              if(dma_buf[i] >= '0' && dma_buf[i] <= '9'){
                                  show_data[i] = dma_buf[i] - '0';
                              }
                              else if (dma_buf[i] >= 'A' && dma_buf[i] <= 'F'){
                                  show_data[i] = dma_buf[i] - 'A' + 10;
                              }
                          }
                          show_data[2] = 0; show_data[3] = 0;
                          printf("OK\r\n");
                      }
                      else{
                          printf("%s", dma_buf);
                      }
                      break;

                  case 5:
                      if(((dma_buf[1] >= '0' && dma_buf[1] <= '9') | (dma_buf[1] >= 'A' && dma_buf[1] <= 'F')) &&
                         ((dma_buf[2] >= '0' && dma_buf[2] <= '9') | (dma_buf[2] >= 'A' && dma_buf[2] <= 'F'))){
                          for (int i = 0; i < 3; ++i) {
                              if(dma_buf[i] >= '0' && dma_buf[i] <= '9'){
                                  show_data[i] = dma_buf[i] - '0';
                              }
                              else if (dma_buf[i] >= 'A' && dma_buf[i] <= 'F'){
                                  show_data[i] = dma_buf[i] - 'A' + 10;
                              }
                          }
                          show_data[3] = 0;
                          printf("OK\r\n");
                      }
                      else{
                          printf("%s", dma_buf);
                      }
                      break;

                  case 6:
                      if(((dma_buf[1] >= '0' && dma_buf[1] <= '9') | (dma_buf[1] >= 'A' && dma_buf[1] <= 'F')) &&
                         ((dma_buf[2] >= '0' && dma_buf[2] <= '9') | (dma_buf[2] >= 'A' && dma_buf[2] <= 'F')) &&
                         ((dma_buf[3] >= '0' && dma_buf[3] <= '9') | (dma_buf[3] >= 'A' && dma_buf[3] <= 'F'))){
                          for (int i = 0; i < 4; ++i) {
                              if(dma_buf[i] >= '0' && dma_buf[i] <= '9'){
                                  show_data[i] = dma_buf[i] - '0';
                              }
                              else if (dma_buf[i] >= 'A' && dma_buf[i] <= 'F'){
                                  show_data[i] = dma_buf[i] - 'A' + 10;
                              }
                          }
                          printf("OK\r\n");
                      }
                      else{
                          printf("%s", dma_buf);
                      }
                      break;

                  default:
                      printf("%s", dma_buf);
                      break;
              }
          }
          else{
              printf("%s", dma_buf);
          }
          recv_len = 0;
      }
#endif
      osDelay(1);
  }
  /* USER CODE END StartUartDMATask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
