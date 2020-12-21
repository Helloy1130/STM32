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
#include "JY901.h"
#include "NRF24L01.h"
#include "usart.h"
#include "string.h"
#include "tim.h"
#include "structconfig.h"
#include "control.h"
#include "pid.h"
#include "adc.h"
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

/* USER CODE END Variables */
/* Definitions for MainTask */
osThreadId_t MainTaskHandle;
const osThreadAttr_t MainTask_attributes = {
  .name = "MainTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};
/* Definitions for IMUTask */
osThreadId_t IMUTaskHandle;
const osThreadAttr_t IMUTask_attributes = {
  .name = "IMUTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for CommunicateTask */
osThreadId_t CommunicateTaskHandle;
const osThreadAttr_t CommunicateTask_attributes = {
  .name = "CommunicateTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartMainTask(void *argument);
void StartIMUTask(void *argument);
void StartCommunicateTask(void *argument);

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

  /* creation of IMUTask */
  IMUTaskHandle = osThreadNew(StartIMUTask, NULL, &IMUTask_attributes);

  /* creation of CommunicateTask */
  CommunicateTaskHandle = osThreadNew(StartCommunicateTask, NULL, &CommunicateTask_attributes);

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
    PidParameter_init(); //PID参数初始化
//    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
//    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
//    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
//    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    uint32_t tick = 0;

    /* Infinite loop */
    for(;;)
    {
        Att_Angle.rol = JY901_getRoll();//姿态角
        Att_Angle.pit = JY901_getPitch();
        Att_Angle.yaw = JY901_getYaw();
        Gyr_rad.X = JY901_getGyroX();//角速度
        Gyr_rad.Y = JY901_getGyroY();
        Gyr_rad.Z = JY901_getGyroZ();
        Control(&Att_Angle,&Gyr_rad,&RC_Control); //姿态控制

        if (osKernelGetTickCount() > tick + 1000){
            HAL_ADC_Start(&hadc1);
            if(HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK){
                printf("VBat = %.2fV\r\n", HAL_ADC_GetValue(&hadc1) * 2 * 3.3 / 4095.0);
            }
            printf("PWM1:%d ", (int)Moto_PWM_1);
            printf("PWM2:%d ", (int)Moto_PWM_2);
            printf("PWM3:%d ", (int)Moto_PWM_3);
            printf("PWM4:%d \r\n", (int)Moto_PWM_4);
            tick = osKernelGetTickCount();
//          printf("-----------Sensor JY901S-----------\r\n");
//
//          printf("AX:%.3f AY:%.3f AZ:%.3f\r\n",JY901_getAccX(),JY901_getAccY(),JY901_getAccZ());
//          printf("WX:%.3f WY:%.3f WZ:%.3f\r\n",Gyr_rad.X,Gyr_rad.Y,Gyr_rad.Z);
//          printf("Roll:%.3f Pitch:%.3f Yaw:%.3f\r\n",Att_Angle.rol,Att_Angle.pit,Att_Angle.yaw);
        }
//      printf("-----------Sensor JY901S-----------\r\n");
//
//      printf("AX:%.3f AY:%.3f AZ:%.3f\r\n",JY901_getAccX(),JY901_getAccY(),JY901_getAccZ());
//      printf("WX:%.3f WY:%.3f WZ:%.3f\r\n",Gyr_rad.X,Gyr_rad.Y,Gyr_rad.Z);
//      printf("Roll:%.3f Pitch:%.3f Yaw:%.3f\r\n",Att_Angle.rol,Att_Angle.pit,Att_Angle.yaw);

        osDelay(10);
    }
  /* USER CODE END StartMainTask */
}

/* USER CODE BEGIN Header_StartIMUTask */
/**
* @brief Function implementing the IMUTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartIMUTask */
void StartIMUTask(void *argument)
{
  /* USER CODE BEGIN StartIMUTask */
    uint8_t rx3_data = 0;
    HAL_UART_Receive_IT(&huart3, &rx3_data, 1);
    /* Infinite loop */
    for(;;)
    {
        if (JY901_recv_flag == true){
//            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            JY901_recv_flag = false;
            if (JY901_readSerialData(rx3_data) == true){
                osDelay(10);
            }
            HAL_UART_Receive_IT(&huart3, &rx3_data, 1);
        }
    }
  /* USER CODE END StartIMUTask */
}

/* USER CODE BEGIN Header_StartCommunicateTask */
/**
* @brief Function implementing the CommunicateTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCommunicateTask */
void StartCommunicateTask(void *argument)
{
  /* USER CODE BEGIN StartCommunicateTask */
    //配置NRF24L01
    uint8_t rx_buf[32] = {0};

    while(NRF24L01_Check()){
        printf("NRF24L01 Check Error\r\n");
    }
    printf("NRF24L01 Check Success\r\n");
    NRF24L01_RX_Mode();

    //配置飞行参数
//    RC_Control.YAW      = 1500;
    RC_Control.THROTTLE = 200;
    RC_Control.ROLL     = 1500;
    RC_Control.PITCH    = 1500;
    /* Infinite loop */
    for(;;)
    {
        if(NRF24L01_RxPacket(rx_buf) == 0){
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            if(rx_buf[0] == 2){
                //降落
//                RC_Control.THROTTLE = 0;
                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
                printf("DOWN\r\n");
            }
            else if(rx_buf[0] == 1){
                HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
                HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
                HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
                HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
                printf("UP\r\n");
//                osDelay(5000);
//                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
//                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
//                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
//                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
            }
        }
        osDelay(50);
    }
  /* USER CODE END StartCommunicateTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
