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
#include <stdlib.h>
#include "tim.h"
#include "GUI.h"
#include "DS_18B20.h"
#include "usart.h"
#include "w25qxx.h"
#include "fatfs.h"
#include "rtc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern GUI_CONST_STORAGE GUI_BITMAP bmCN_bilibiliB;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_12;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SET_ADDR (10 * 4096)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
bool beep_switch = false;
bool saveUartData_flag = false;

float temp;
int temp_max = 40;
int alarm_time = 5;
/* USER CODE END Variables */
/* Definitions for MainTask */
osThreadId_t MainTaskHandle;
const osThreadAttr_t MainTask_attributes = {
        .name = "MainTask",
        .priority = (osPriority_t) osPriorityAboveNormal,
        .stack_size = 2048 * 4
};
/* Definitions for keyTask */
osThreadId_t keyTaskHandle;
const osThreadAttr_t keyTask_attributes = {
        .name = "keyTask",
        .priority = (osPriority_t) osPriorityNormal,
        .stack_size = 2048 * 4
};
/* Definitions for beepTask */
osThreadId_t beepTaskHandle;
const osThreadAttr_t beepTask_attributes = {
        .name = "beepTask",
        .priority = (osPriority_t) osPriorityNormal,
        .stack_size = 2048 * 4
};
/* Definitions for GUITask */
osThreadId_t GUITaskHandle;
const osThreadAttr_t GUITask_attributes = {
        .name = "GUITask",
        .priority = (osPriority_t) osPriorityLow,
        .stack_size = 2048 * 4
};
/* Definitions for UartTask */
osThreadId_t UartTaskHandle;
const osThreadAttr_t UartTask_attributes = {
        .name = "UartTask",
        .priority = (osPriority_t) osPriorityNormal,
        .stack_size = 2048 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void SaveAlarmSet(uint32_t addr, int setval);

int LoadAlarmSet(uint32_t addr);

void SavePara(void);

void LoadPara(void);

void SaveAlarm(float alarm_temp);

void PrintfAlarm(void);

void SaveUartData(uint8_t *recv, int len);

void PrintfUartData(void);

/* USER CODE END FunctionPrototypes */

void StartMainTask(void *argument);

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
    /* creation of MainTask */
    MainTaskHandle = osThreadNew(StartMainTask, NULL, &MainTask_attributes);

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

/* USER CODE BEGIN Header_StartMainTask */
/**
  * @brief  Function implementing the MainTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMainTask */
void StartMainTask(void *argument) {
    /* USER CODE BEGIN StartMainTask */
    ds18b20_init();
    W25QXX_Init();
    printf("W25QXX FLASH ID = 0x%04X\r\n", W25QXX_ReadID());
    int val = LoadAlarmSet(SET_ADDR);
    if (val > -55 && val < 125) {
        temp_max = val;
        printf("temp max = %d C\r\n", temp_max);
    }

    LoadPara();

    uint32_t tick = 0;
    uint32_t tick2 = 0;
    /* Infinite loop */
    for (;;) {
        if (osKernelGetTickCount() >= tick) {//100ms 读取一次
            tick = osKernelGetTickCount() + 100;
            temp = ds18b20_read();
//            printf("temperature = %5.1f C\r\n", temp);

            if (temp > temp_max) {
                beep_switch = true;
            } else {
                beep_switch = false;
            }
        }

        if (osKernelGetTickCount() > tick2 + 1000) {
            tick2 = osKernelGetTickCount();
            ReadRTCDateTime();
        }

        osDelay(1);
    }
    /* USER CODE END StartMainTask */
}

/* USER CODE BEGIN Header_StartKeyTask */
/**
* @brief Function implementing the keyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKeyTask */
void StartKeyTask(void *argument) {
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
            case 1://增加温度上限
                temp_max++;
                SaveAlarmSet(SET_ADDR, temp_max);
                SavePara();
                break;

            case 2://减少温度上限
                temp_max--;
                SaveAlarmSet(SET_ADDR, temp_max);
                SavePara();
                break;

            case 3://打印报警记录
                PrintfAlarm();
                break;

            case 4://存储串口数据
                f_unlink("0:/recv.txt");
                printf("Please send data from uart\r\n");
                saveUartData_flag = true;
                break;

            case 5://打印存储的串口数据
                if (saveUartData_flag == false) {
                    PrintfUartData();
                }
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

/* USER CODE BEGIN Header_StartBeepTask */
/**
* @brief Function implementing the beepTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBeepTask */
void StartBeepTask(void *argument) {
    /* USER CODE BEGIN StartBeepTask */
    uint32_t tick = osKernelGetTickCount() + 5000;
    /* Infinite loop */
    for (;;) {
        if (beep_switch == true) {
            if (osKernelGetTickCount() >= tick) {//报警一次嘀嘀嘀
                for (int i = 0; i < 3; ++i) {
                    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);//使能输出
                    osDelay(100);
                    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
                    osDelay(100);
                }
                printf("The current temperature is too high!(%5.1f C)\r\n", temp);
                SaveAlarm(temp);
                tick = osKernelGetTickCount() + alarm_time * 1000;
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
/* USER CODE END Header_StartGUITask */
void StartGUITask(void *argument) {
    /* USER CODE BEGIN StartGUITask */
    char show_data[30];

    GUI_Init();
    GUI_DrawBitmap(&bmCN_bilibiliB, 0, 0);
    GUI_Update();//刷新显示
    osDelay(1000);
    GUI_Clear();
    GUI_SetFont(&GUI_FontHZ_SimSun_12);
    GUI_DispString("学号姓名界面\n");
    GUI_DispStringAt("学号：18042209\n", 0, 20);
    GUI_DispString("姓名：陈俊煜\n");
    GUI_DispString("课程：嵌入式系统实验\n");
    GUI_Update();//刷新显示
    osDelay(1000);

    uint32_t tick = osKernelGetTickCount() + 100;
    /* Infinite loop */
    for (;;) {
        if (osKernelGetTickCount() >= tick) {//100ms 显示一次
            tick = osKernelGetTickCount() + 100;
            GUI_Clear();
            sprintf(show_data, "%d.%d.%d %02d:%02d:%02d", RTC_Year, RTC_Mon, RTC_Dat, RTC_Hour, RTC_Min, RTC_Sec);
            GUI_DispStringAt(show_data, 0, 0);
            sprintf(show_data, "温度：%5.1f ℃", temp);
            GUI_DispStringAt(show_data, 0, 16);
            sprintf(show_data, "<温度上限>：%3d ℃", temp_max);
            GUI_DispStringAt(show_data, 0, 32);
            sprintf(show_data, "<报警间隔>：%3d 秒", alarm_time);
            GUI_DispStringAt(show_data, 0, 48);
            GUI_Update();//刷新显示
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
void StartUartTask(void *argument) {
    /* USER CODE BEGIN StartUartTask */
    uint32_t tick = 0;

    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);//使能idle中断
    HAL_UART_Receive_DMA(&huart1, dma_buf, MAX_BUF_LEN);
    /* Infinite loop */
    for (;;) {
        if (saveUartData_flag) {
            if (recv_len > 0) {
                tick = osKernelGetTickCount();
//                printf("%s", dma_buf);
                SaveUartData(dma_buf, recv_len);
                recv_len = 0;
            }
            if (osKernelGetTickCount() > tick + 1000 && tick > 0) {
                tick = 0;
                saveUartData_flag = false;
            }
        } else {
            if (recv_len > 0) {
//          printf("%s", dma_buf);
                if (strstr((char *) dma_buf, "Set Temp Max = ") != NULL) {
                    int recv_temp_max = atoi((char *) dma_buf + 15);
                    if (recv_temp_max >= -10 && recv_temp_max <= 100) {
                        temp_max = recv_temp_max;
                        SavePara();
                    } else {
                        printf("Please check the input! (Temp Max min is -10 and max is 100)\r\n");
                    }
                } else if (strstr((char *) dma_buf, "Set Alarm time = ") != NULL) {
                    int time = atoi((char *) dma_buf + 17);
                    if (time >= 1 && time <= 60) {
                        alarm_time = time;
                        SavePara();
                    } else {
                        printf("Please check the input! (Alarm time min is 1 and max is 60)\r\n");
                    }
                } else {
                    printf("Please check the input! (Example:Set Temp Max = 40 or Set Alarm time = 5)\r\n");
                }
                recv_len = 0;
            }
        }
        osDelay(1);
    }
    /* USER CODE END StartUartTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void SaveAlarmSet(uint32_t addr, int setval) {
    W25QXX_Write((uint8_t *) (&setval), addr, sizeof(setval));
}

int LoadAlarmSet(uint32_t addr) {
    int val = 0;
    W25QXX_Read((uint8_t *) (&val), addr, sizeof(val));

    return val;
}

void SavePara(void) {
    FIL fil;//文件对象
    FRESULT res;//操作返回结果

    res = f_open(&fil, "0:/para.txt", FA_WRITE | FA_OPEN_ALWAYS);
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        f_lseek(&fil, 0);
        f_printf(&fil, "ALARM_MAX:%d\r\n", temp_max);
        f_printf(&fil, "ALARM_TIME:%d\r\n", alarm_time);
        f_close(&fil);
    }
}

void LoadPara(void) {
    FIL fil;//文件对象
    FRESULT res;//操作返回结果
    char buf[50];

    res = f_open(&fil, "0:/para.txt", FA_READ | FA_OPEN_ALWAYS);
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        f_lseek(&fil, 0);
        while (f_gets(buf, 50, &fil)) {
            if (strstr(buf, "ALARM_MAX:") != NULL) {
                int recv_temp_max = atoi(buf + 10);
                if (recv_temp_max >= -10 && recv_temp_max <= 100) {
                    temp_max = recv_temp_max;
                } else {
                    printf("temp_max load error!\r\n");
                }
            } else if (strstr(buf, "ALARM_TIME:") != NULL) {
                int time = atoi(buf + 11);
                if (time >= 1 && time <= 60) {
                    alarm_time = time;
                } else {
                    printf("alarm_time load error!\r\n");
                }
            } else {
                printf("load error!\r\n");
            }
        }
        f_close(&fil);
    }
}

void SaveAlarm(float alarm_temp) {
    FIL fil;//文件对象
    FRESULT res;//操作返回结果
    char buf[100];

    res = f_open(&fil, "0:/alarm.txt", FA_WRITE | FA_OPEN_APPEND);
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        sprintf(buf, "%5.1f C(%4d.%2d.%2d %02d:%02d:%02d)\r\n", alarm_temp, RTC_Year, RTC_Mon, RTC_Dat, RTC_Hour,
                RTC_Min, RTC_Sec);
        f_puts(buf, &fil);
        f_close(&fil);
    }
}

void PrintfAlarm(void) {
    FIL fil;//文件对象
    FRESULT res;//操作返回结果
    char buf[100];

    res = f_open(&fil, "0:/alarm.txt", FA_READ | FA_OPEN_ALWAYS);
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        f_lseek(&fil, 0);
        while (f_gets(buf, 100, &fil)) {
            char temp_buf[10];
            strncpy(temp_buf, buf + 19, 2);
            int temp_hour = atoi(temp_buf);
            strncpy(temp_buf, buf + 22, 2);
            int temp_min = atoi(temp_buf);
            strncpy(temp_buf, buf + 25, 2);
            int temp_sec = atoi(temp_buf);

            uint32_t rtc_time = RTC_Hour * 3600 + RTC_Min * 60 + RTC_Sec;
            uint32_t temp_time = temp_hour * 3600 + temp_min * 60 + temp_sec;
            if (rtc_time - temp_time <= 60) {
                printf("%s", buf);
            }
        }
        f_close(&fil);
    }
}

void SaveUartData(uint8_t *recv, int len) {
    FIL fil;//文件对象
    FRESULT res;//操作返回结果

    res = f_open(&fil, "0:/recv.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_OPEN_APPEND);
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        UINT bw;
        f_write(&fil, recv, len, &bw);
        f_close(&fil);
    }
}

void PrintfUartData(void) {
    FIL fil;//文件对象
    FRESULT res;//操作返回结果
    char buf[200];

    res = f_open(&fil, "0:/recv.txt", FA_READ | FA_OPEN_ALWAYS);
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        f_lseek(&fil, 0);
        while (f_gets(buf, 200, &fil)) {
            printf("%s\r\n", buf);
        }
        f_close(&fil);
    }
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
