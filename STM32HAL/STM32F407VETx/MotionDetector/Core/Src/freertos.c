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
#include "MPU6050.h"
#include "ESP8266.h"
#include "usart.h"
#include "tim.h"
#include "DigitalTube.h"
#include "rtc.h"
#include "fatfs.h"
#include "w25qxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//WIFI����
#define WIFI_NAME       "MotionDetector"
#define WIFI_PASSWD     "12345678"
#define UDP_IP          "192.168.4.2"
#define UDP_PORT        9000
//W25QXX FLASH ������¼
#define SET_ADDR (10 * 4096)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern uint32_t g_time_count;

uint8_t g_seg_show = 2;//�����Ĭ����ʾ�롢�ٷ���
uint8_t g_timetxt_count = 0;
bool g_mpu_start = false;
bool g_mpu_save = false;
char g_mpu_data[300][100];
/* USER CODE END Variables */
/* Definitions for MainTask */
osThreadId_t MainTaskHandle;
const osThreadAttr_t MainTask_attributes = {
  .name = "MainTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 2048 * 4
};
/* Definitions for KeyTask */
osThreadId_t KeyTaskHandle;
const osThreadAttr_t KeyTask_attributes = {
  .name = "KeyTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 4096 * 4
};
/* Definitions for DigitaTubeTask */
osThreadId_t DigitaTubeTaskHandle;
const osThreadAttr_t DigitaTubeTask_attributes = {
  .name = "DigitaTubeTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for RTCTask */
osThreadId_t RTCTaskHandle;
const osThreadAttr_t RTCTask_attributes = {
  .name = "RTCTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void SaveTime(void);

void PrintfTime(void);

void SaveTimeCount(uint32_t addr, int data);

int LoadTimeCount(uint32_t addr);

void SaveData(void);

void PrintfData(void);

/* USER CODE END FunctionPrototypes */

void StartMainTask(void *argument);
void StartKeyTask(void *argument);
void StartDigitaTubeTask(void *argument);
void StartRTCTask(void *argument);

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

  /* creation of KeyTask */
  KeyTaskHandle = osThreadNew(StartKeyTask, NULL, &KeyTask_attributes);

  /* creation of DigitaTubeTask */
  DigitaTubeTaskHandle = osThreadNew(StartDigitaTubeTask, NULL, &DigitaTubeTask_attributes);

  /* creation of RTCTask */
  RTCTaskHandle = osThreadNew(StartRTCTask, NULL, &RTCTask_attributes);

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
    uint32_t tick = 0;
    int mpu_data_count = 0;

    float SelfTest[6];               // Gyro and accelerometer self-test sensor output
    float aRes, gRes;                // scale resolutions per LSB for the sensors
    int16_t accelCount[3];           // Stores the 16-bit signed accelerometer sensor output
    float ax, ay, az;                // Stores the real accel value in g's
    int16_t gyroCount[3];            // Stores the 16-bit signed gyro sensor output
    float gx, gy, gz;                // Stores the real gyro value in degrees per seconds
    float gyroBias[3], accelBias[3]; // Bias corrections for gyro and accelerometer
    int16_t tempCount;               // Stores the internal chip temperature sensor output
    float temperature;               // Scaled temperature in degrees Celsius

    if (MPU6050_Check() == MPU6050_ADDRESS) {
        MPU6050_SelfTest(SelfTest); // Start by performing self test and reporting values
        printf("x-axis self test: acceleration trim within : ");
        printf("%.1f", SelfTest[0]);
        printf("%% of factory value\r\n");
        printf("y-axis self test: acceleration trim within : ");
        printf("%.1f", SelfTest[1]);
        printf("%% of factory value\r\n");
        printf("z-axis self test: acceleration trim within : ");
        printf("%.1f", SelfTest[2]);
        printf("%% of factory value\r\n");
        printf("x-axis self test: gyration trim within : ");
        printf("%.1f", SelfTest[3]);
        printf("%% of factory value\r\n");
        printf("y-axis self test: gyration trim within : ");
        printf("%.1f", SelfTest[4]);
        printf("%% of factory value\r\n");
        printf("z-axis self test: gyration trim within : ");
        printf("%.1f", SelfTest[5]);
        printf("%% of factory value\r\n");

        if (SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f &&
            SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
            printf("Pass Selftest!\r\n");

            MPU6050_calibrate(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers
            MPU6050_init();
            printf("MPU6050 initialized for active data mode....\r\n"); // Initialize device for active mode read of acclerometer, gyroscope, and temperature
        } else {
            printf("Could not connect to MPU6050\r\n");
        }
    }

    HAL_UART_Receive_IT(&huart6, &rx_byte, 1);

    if (ESP8266_Init()) {
        printf("ESP8266 Init OK!\r\n");
    } else {
        printf("ESP8266 Init FAIL!\r\n");
    }

    if (ESP8266_AP(WIFI_NAME, WIFI_PASSWD)) {
        printf("ESP8266 Create AP OK!\r\n");
    } else {
        printf("ESP8266 Create AP FAIL!\r\n");
    }
//    if (ESP8266_UDP_Connect(UDP_IP,UDP_PORT)) {
//        printf("ESP8266 Create UDP OK!\r\n");
//    } else {
//        printf("ESP8266 Create UDP FAIL!\r\n");
//    }

    /* Infinite loop */
    for (;;) {
        if (g_mpu_start == true) {
            if (MPU6050_readByte(MPU6050_ADDRESS, INT_STATUS) & 0x01) {  // check if data ready interrupt
                MPU6050_readAccelData(accelCount);  // Read the x/y/z adc values
                aRes = MPU6050_getAres();

                // Now we'll calculate the accleration value into actual g's
                ax = (float) accelCount[0] * aRes -
                     accelBias[0];  // get actual g value, this depends on scale being set
                ay = (float) accelCount[1] * aRes - accelBias[1];
                az = (float) accelCount[2] * aRes - accelBias[2];

                MPU6050_readGyroData(gyroCount);  // Read the x/y/z adc values
                gRes = MPU6050_getGres();

                // Calculate the gyro value into actual degrees per second
                gx = (float) gyroCount[0] * gRes -
                     gyroBias[0];  // get actual gyro value, this depends on scale being set
                gy = (float) gyroCount[1] * gRes - gyroBias[1];
                gz = (float) gyroCount[2] * gRes - gyroBias[2];

                tempCount = MPU6050_readTempData();  // Read the x/y/z adc values
                temperature = ((float) tempCount) / 340. + 36.53; // Temperature in degrees Centigrade

                uint8_t test_min = (g_time_count / 100) / 60;
                uint8_t test_sec = (g_time_count / 100) % 60;
                uint8_t test_sec_100 = g_time_count % 100;
                sprintf(g_mpu_data[mpu_data_count],
                        "ax:%.1f,ay:%.1f,az:%.1f mg gx:%.1f,gy:%.1f,gz:%.1f deg/s (%02d:%02d:%02d)\r\n",
                        1000 * ax, 1000 * ay, 1000 * az, gx, gy, gz, test_min, test_sec, test_sec_100);
                printf("%s",g_mpu_data[mpu_data_count]);
                mpu_data_count++;
            }
        }

        if (g_mpu_save == true) {
            g_mpu_save = false;
            mpu_data_count = 0;
            SaveData();
            for (int i = 0; i < 300; ++i) {
                strcpy(g_mpu_data[i], (char *)'\0');
                printf("%s", g_mpu_data[i]);
            }
        }
        /*       if (osKernelGetTickCount() - tick > 500) {//500ms
                   // Print acceleration values in milligs!
                   printf("X-acceleration: ");
                   printf("%.1f", 1000 * ax);
                   printf(" mg ");
                   printf("Y-acceleration: ");
                   printf("%.1f", 1000 * ay);
                   printf(" mg ");
                   printf("Z-acceleration: ");
                   printf("%.1f", 1000 * az);
                   printf(" mg\r\n");

                   // Print gyro values in degree/sec
                   printf("X-gyro rate: ");
                   printf("%.1f", gx);
                   printf(" degrees/sec ");
                   printf("Y-gyro rate: ");
                   printf("%.1f", gy);
                   printf(" degrees/sec ");
                   printf("Z-gyro rate: ");
                   printf("%.1f", gz);
                   printf(" degrees/sec\r\n");

                   // Print temperature in degrees Centigrade
                   printf("Temperature is ");
                   printf("%.2f", temperature);
                   printf(" degrees C\r\n"); // Print T values to tenths of s degree C
                   printf("\r\n");

                   tick = osKernelGetTickCount();
               }*/
        osDelay(50);
    }
  /* USER CODE END StartMainTask */
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

    W25QXX_Init();
    printf("W25QXX FLASH ID = 0x%04X\r\n", W25QXX_ReadID());
    g_timetxt_count = LoadTimeCount(SET_ADDR);
    printf("Time Count = %lu\r\n", g_time_count);
    /* Infinite loop */
    for (;;) {
        //ɨ�谴��
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

        //������������
        switch (key_state) {
            case 1://��ȡ����ʱ���¼
                PrintfTime();
                break;
            case 2:
                g_seg_show = 1;//�������ʾ�֡���
                break;
            case 3:
                g_seg_show = 2;//�������ʾ�롢�ٷ���
                break;
            case 4:
                PrintfData();
                break;
            case 5://���㲢��ʼ��ʱ
                g_time_count = 0;
                HAL_TIM_Base_Start_IT(&htim3);
                g_mpu_start = true;
                break;
            case 6://ֹͣ��ʱ����¼
                HAL_TIM_Base_Stop_IT(&htim3);
                g_mpu_start = false;
                g_mpu_save = true;
                if (g_timetxt_count >= 100) {//����100������ʱ�����ļ�
                    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
                    FIL fil;//�ļ�����
                    FRESULT res;//�������ؽ��
                    res = f_open(&fil, "0:/time.txt", FA_CREATE_ALWAYS);
                    if (res != FR_OK && res != FR_EXIST) {
                        printf("Save time error.\r\n");
                    } else {
                        printf("The time record data has exceeded 100. This file has been overwritten for you.\r\n");
                        f_close(&fil);
                    }
                    g_timetxt_count = 0;
                    osDelay(1000);
                    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
                } else {
                    SaveTime();
                    g_timetxt_count++;
                    SaveTimeCount(SET_ADDR, g_timetxt_count);
                    printf("Save time OK! Now time.txt have %d data.\r\n", g_timetxt_count);
                }
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
        if (g_seg_show == 1) {//�֡���
            seg[0] = ((g_time_count / 100) / 60) / 10;
            seg[1] = ((g_time_count / 100) / 60) % 10;
            seg[2] = ((g_time_count / 100) % 60) / 10;
            seg[3] = ((g_time_count / 100) % 60) % 10;
        } else if (g_seg_show == 2) {//�롢�ٷ���
            seg[0] = ((g_time_count / 100) % 60) / 10;
            seg[1] = ((g_time_count / 100) % 60) % 10;
            seg[2] = (g_time_count % 100) / 10;
            seg[3] = g_time_count % 10;
        }
        for (int i = 0; i < 4; ++i) {
            if (i == 1 && osKernelGetTickCount() % 1000 < 500) {
                Write595(i, seg[i], 1);
            } else {
                Write595(i, seg[i], 0);
            }
            osDelay(5);
        }
    }
  /* USER CODE END StartDigitaTubeTask */
}

/* USER CODE BEGIN Header_StartRTCTask */
/**
* @brief Function implementing the RTCTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRTCTask */
void StartRTCTask(void *argument)
{
  /* USER CODE BEGIN StartRTCTask */
    uint32_t tick = 0;
    /* Infinite loop */
    for (;;) {
        if (osKernelGetTickCount() - tick >= 1000) {
            tick = osKernelGetTickCount();
            ReadRTCDateTime();
        }
        osDelay(1);
    }
  /* USER CODE END StartRTCTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void SaveTime(void) {
    FIL fil;//�ļ�����
    FRESULT res;//�������ؽ��
    char buf[100];

    res = f_open(&fil, "0:/time.txt", FA_WRITE | FA_OPEN_APPEND);
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        uint8_t test_min = (g_time_count / 100) / 60;
        uint8_t test_sec = (g_time_count / 100) % 60;
        uint8_t test_sec_100 = g_time_count % 100;
        sprintf(buf, "Test Time:%02d:%02d:%02d (%4d.%2d.%2d %02d:%02d:%02d)\r\n", test_min, test_sec, test_sec_100,
                RTC_Year, RTC_Mon, RTC_Dat, RTC_Hour,
                RTC_Min, RTC_Sec);
        f_puts(buf, &fil);
        f_close(&fil);
    }
}

void PrintfTime(void) {
    FIL fil;//�ļ�����
    FRESULT res;//�������ؽ��
    char buf[100];

    res = f_open(&fil, "0:/time.txt", FA_READ | FA_OPEN_ALWAYS);
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        f_lseek(&fil, 0);
        while (f_gets(buf, 100, &fil)) {
            printf("%s", buf);
        }
        f_close(&fil);
    }
}

void SaveTimeCount(uint32_t addr, int data) {
    W25QXX_Write((uint8_t *) (&data), addr, sizeof(data));
}

int LoadTimeCount(uint32_t addr) {
    int data = 0;
    W25QXX_Read((uint8_t *) (&data), addr, sizeof(data));

    return data;
}

void SaveData(void) {
    FIL fil;//�ļ�����
    FRESULT res;//�������ؽ��

//    res = f_open(&fil, "0:/data.txt", FA_CREATE_ALWAYS);
//    if (res != FR_OK && res != FR_EXIST) {
//        printf("create file error.\r\n");
//    } else {
//        printf("create data.txt ok\r\n");
//        f_close(&fil);
//    }
    res = f_open(&fil, "0:/data.txt", FA_OPEN_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        for (int i = 0; i < 300; ++i) {
            f_puts(g_mpu_data[i], &fil);
        }
        printf("Save MPU Data OK!\r\n");
        f_close(&fil);
    }
}

void PrintfData(void) {
    FIL fil;//�ļ�����
    FRESULT res;//�������ؽ��
    char buf[100];

    res = f_open(&fil, "0:/data.txt", FA_READ | FA_OPEN_ALWAYS);
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        f_lseek(&fil, 0);
        while (f_gets(buf, 100, &fil)) {
            printf("%s", buf);
        }
        f_close(&fil);
    }
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
