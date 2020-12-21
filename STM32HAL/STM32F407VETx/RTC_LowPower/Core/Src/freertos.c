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
#include "rtc.h"
#include "DigitalTube.h"
#include "tim.h"
#include "LED.h"
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
uint8_t seg[4];//数码管

bool beep_switch = false;

bool led_switch = true;
uint32_t led_control_time = 200;

bool rtc_setting_switch = false;
uint8_t rtc_show_flag = 4;

bool low_power_swich = false;
/* USER CODE END Variables */
/* Definitions for RTCTask */
osThreadId_t RTCTaskHandle;
const osThreadAttr_t RTCTask_attributes = {
  .name = "RTCTask",
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
/* Definitions for DigitaTubeTask */
osThreadId_t DigitaTubeTaskHandle;
const osThreadAttr_t DigitaTubeTask_attributes = {
  .name = "DigitaTubeTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for LedTask */
osThreadId_t LedTaskHandle;
const osThreadAttr_t LedTask_attributes = {
  .name = "LedTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void EnterSleepMode(void);

void EnterStopMode(void);

/* USER CODE END FunctionPrototypes */

void StartRTCTask(void *argument);
void StartKeyTask(void *argument);
void StartBeepTask(void *argument);
void StartDigitaTubeTask(void *argument);
void StartLedTask(void *argument);

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
  /* creation of RTCTask */
  RTCTaskHandle = osThreadNew(StartRTCTask, NULL, &RTCTask_attributes);

  /* creation of KeyTask */
  KeyTaskHandle = osThreadNew(StartKeyTask, NULL, &KeyTask_attributes);

  /* creation of BeepTask */
  BeepTaskHandle = osThreadNew(StartBeepTask, NULL, &BeepTask_attributes);

  /* creation of DigitaTubeTask */
  DigitaTubeTaskHandle = osThreadNew(StartDigitaTubeTask, NULL, &DigitaTubeTask_attributes);

  /* creation of LedTask */
  LedTaskHandle = osThreadNew(StartLedTask, NULL, &LedTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartRTCTask */
/**
  * @brief  Function implementing the RTCTask thread.
  * @param  argument: Not used
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
            if (ReadRTCDateTime() == HAL_OK) {//每秒读取一次RTC时间
                switch (rtc_show_flag) {
                    case 1:
                        seg[0] = RTC_Year / 1000;
                        seg[1] = (RTC_Year % 1000) / 100;
                        seg[2] = (RTC_Year % 100) / 10;
                        seg[3] = RTC_Year % 10;
                        break;

                    case 2:
                        seg[0] = RTC_Mon / 10;
                        seg[1] = RTC_Mon % 10;
                        seg[2] = RTC_Dat / 10;
                        seg[3] = RTC_Dat % 10;
                        break;

                    case 3:
                        seg[0] = RTC_Hour / 10;
                        seg[1] = RTC_Hour % 10;
                        seg[2] = RTC_Min / 10;
                        seg[3] = RTC_Min % 10;
                        break;

                    case 4:
                        seg[0] = RTC_Min / 10;
                        seg[1] = RTC_Min % 10;
                        seg[2] = RTC_Sec / 10;
                        seg[3] = RTC_Sec % 10;
                        break;

                    default:
                        break;
                }
            }
        }
        osDelay(1);
    }
  /* USER CODE END StartRTCTask */
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
    uint32_t key_tick = 0;
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

        if (low_power_swich == true) {
            if (osKernelGetTickCount() > key_tick + 10 * 1000) {
                __HAL_TIM_SET_AUTORELOAD(&htim3, 549);
                beep_switch = true;
                osDelay(100);
//                EnterStopMode();
                EnterSleepMode();
                key_tick = osKernelGetTickCount();
            } else if (key_state > 0) {
                key_tick = osKernelGetTickCount();
            }
        }
        //按键触发控制
        switch (key_state) {
            case 1:
                if (rtc_setting_switch == true) {
                    switch (rtc_show_flag) {
                        case 1:
                            RTC_Year++;
                            SetRTCDate(RTC_Year, RTC_Mon, RTC_Dat);
                            break;

                        case 2:
                            if (RTC_Mon < 12) {
                                RTC_Mon++;
                            } else {
                                RTC_Mon = 1;
                            }
                            SetRTCDate(RTC_Year, RTC_Mon, RTC_Dat);
                            break;

                        case 3:
                            if (RTC_Hour < 23) {
                                RTC_Hour++;
                            } else {
                                RTC_Hour = 0;
                            }
                            SetRTCTime(RTC_Hour, RTC_Min, RTC_Sec);
                            break;

                        case 4:
                            if (RTC_Min < 59) {
                                RTC_Min++;
                            } else {
                                RTC_Min = 0;
                            }
                            SetRTCTime(RTC_Hour, RTC_Min, RTC_Sec);
                            break;

                        default:
                            break;
                    }
                } else {
                    rtc_show_flag = 1;
                }
                break;

            case 2:
                if (rtc_setting_switch == true) {
                    switch (rtc_show_flag) {
                        case 2:
                            if (RTC_Dat > 1) {
                                RTC_Dat--;
                            } else {
                                RTC_Dat = 31;
                            }
                            SetRTCDate(RTC_Year, RTC_Mon, RTC_Dat);
                            break;

                        case 3:
                            if (RTC_Min > 0) {
                                RTC_Min--;
                            } else {
                                RTC_Min = 59;
                            }
                            SetRTCTime(RTC_Hour, RTC_Min, RTC_Sec);
                            break;

                        case 4:
                            if (RTC_Sec > 0) {
                                RTC_Sec--;
                            } else {
                                RTC_Sec = 59;
                            }
                            SetRTCTime(RTC_Hour, RTC_Min, RTC_Sec);
                            break;

                        default:
                            break;
                    }
                } else {
                    rtc_show_flag = 2;
                }
                break;

            case 3:
                if (rtc_setting_switch == true) {
                    switch (rtc_show_flag) {
                        case 2:
                            if (RTC_Dat < 31) {
                                RTC_Dat++;
                            } else {
                                RTC_Dat = 1;
                            }
                            SetRTCDate(RTC_Year, RTC_Mon, RTC_Dat);
                            break;

                        case 3:
                            if (RTC_Min < 59) {
                                RTC_Min++;
                            } else {
                                RTC_Min = 0;
                            }
                            SetRTCTime(RTC_Hour, RTC_Min, RTC_Sec);
                            break;

                        case 4:
                            if (RTC_Sec < 59) {
                                RTC_Sec++;
                            } else {
                                RTC_Sec = 0;
                            }
                            SetRTCTime(RTC_Hour, RTC_Min, RTC_Sec);
                            break;

                        default:
                            break;
                    }
                } else {
                    rtc_show_flag = 3;
                }
                break;

            case 4:
                if (rtc_setting_switch == true) {
                    switch (rtc_show_flag) {
                        case 1:
                            RTC_Year--;
                            SetRTCDate(RTC_Year, RTC_Mon, RTC_Dat);
                            break;

                        case 2:
                            if (RTC_Mon > 1) {
                                RTC_Mon--;
                            } else {
                                RTC_Mon = 12;
                            }
                            SetRTCDate(RTC_Year, RTC_Mon, RTC_Dat);
                            break;

                        case 3:
                            if (RTC_Hour > 0) {
                                RTC_Hour--;
                            } else {
                                RTC_Hour = 23;
                            }
                            SetRTCTime(RTC_Hour, RTC_Min, RTC_Sec);
                            break;

                        case 4:
                            if (RTC_Min > 0) {
                                RTC_Min--;
                            } else {
                                RTC_Min = 59;
                            }
                            SetRTCTime(RTC_Hour, RTC_Min, RTC_Sec);
                            break;

                        default:
                            break;
                    }
                } else {
                    rtc_show_flag = 4;
                }
                break;

            case 5://进入/退出RTC时间配置模式
                if (low_power_swich == false){
                    rtc_setting_switch = !rtc_setting_switch;
                }
                break;

            case 6://进入/关闭节电模式
                low_power_swich = !low_power_swich;
                if (low_power_swich == true) {
                    key_tick = osKernelGetTickCount();
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
    for (;;) {
        if (beep_switch == true) {
            HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);//使能输出
            osDelay(100);
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
            beep_switch = false;
        }
        osDelay(1);
    }
  /* USER CODE END StartBeepTask */
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
    /* Infinite loop */
    for (;;) {
        for (int i = 0; i < 4; ++i) {
            if (rtc_show_flag == 4) {
                if (rtc_setting_switch == true) {
                    if (osKernelGetTickCount() % 1000 < 300) {
                        Write595(i, 0xff, 0);
                    } else {
                        if (i == 1) {
                            Write595(i, seg[i], 1);
                        } else {
                            Write595(i, seg[i], 0);
                        }
                    }
                } else if (low_power_swich == true) {
                    if (osKernelGetTickCount() % 1000 < 50) {
                        Write595(i, 0xff, 0);
                    } else {
                        if (i == 1 && osKernelGetTickCount() % 1000 < 500) {
                            Write595(i, seg[i], 1);
                        } else {
                            Write595(i, seg[i], 0);
                        }
                    }
                } else {
                    if (i == 1 && osKernelGetTickCount() % 1000 < 500) {
                        Write595(i, seg[i], 1);
                    } else {
                        Write595(i, seg[i], 0);
                    }
                }
            } else if (rtc_show_flag == 1) {
                if (rtc_setting_switch == true) {
                    if (osKernelGetTickCount() % 1000 < 300) {
                        Write595(i, 0xff, 0);
                    } else {
                        Write595(i, seg[i], 0);
                    }
                } else if (low_power_swich == true) {
                    if (osKernelGetTickCount() % 1000 < 50) {
                        Write595(i, 0xff, 0);
                    } else {
                        Write595(i, seg[i], 0);
                    }
                } else {
                    Write595(i, seg[i], 0);
                }
            } else {
                if (rtc_setting_switch == true) {
                    if (osKernelGetTickCount() % 1000 < 300) {
                        Write595(i, 0xff, 0);
                    } else {
                        if (i == 1) {
                            Write595(i, seg[i], 1);
                        } else {
                            Write595(i, seg[i], 0);
                        }
                    }
                } else if (low_power_swich == true) {
                    if (osKernelGetTickCount() % 1000 < 50) {
                        Write595(i, 0xff, 0);
                    } else {
                        if (i == 1) {
                            Write595(i, seg[i], 1);
                        } else {
                            Write595(i, seg[i], 0);
                        }
                    }
                } else {
                    if (i == 1) {
                        Write595(i, seg[i], 1);
                    } else {
                        Write595(i, seg[i], 0);
                    }
                }
            }
            osDelay(5);
        }
    }
  /* USER CODE END StartDigitaTubeTask */
}

/* USER CODE BEGIN Header_StartLedTask */
/**
* @brief Function implementing the LedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLedTask */
void StartLedTask(void *argument)
{
  /* USER CODE BEGIN StartLedTask */
    uint8_t led_id = 1;
    /* Infinite loop */
    for (;;) {
        if (led_switch) {
            turnoffLED();
            selectLED(led_id);
            if (led_id == 8) {
                led_id = 1;
            } else {
                led_id++;
            }
            osDelay(led_control_time);
        }
        osDelay(1);
    }
  /* USER CODE END StartLedTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void EnterSleepMode(void) {
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 10, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);//开启RTC唤醒中断,10秒后唤醒
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//关闭滴答定时器
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);//开始睡眠模式
}

void EnterStopMode(void) {
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 10, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);//开启RTC唤醒中断,10秒后唤醒
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//关闭滴答定时器
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);//开始停止模式
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
