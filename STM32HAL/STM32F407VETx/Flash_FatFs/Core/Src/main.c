/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "fatfs.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "w25qxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define testW25Q128 0
#define testFATFS 0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void TestW25Q128(void) {
    uint8_t dat[4096] = "1234567890-1234567890-1234567890-1234567890-1234567890-\0";
    uint8_t read_buf[4096] = {0};
    uint16_t i;
    uint16_t *pDat = (uint16_t *) dat;
    for (i = 0; i < sizeof(dat) / 2; ++i) {
        pDat[i] = i * i;
    }

    W25QXX_Init(); //初始化FLASH器件，读取器件ID，以此判断器件型号
    uint16_t device_id = W25QXX_ReadID();
    printf("device_id = 0x%04X\r\n", device_id);
    HAL_Delay(500);
    uint32_t addr;
    for (int j = 0; j < 1; ++j) {//只测试一个扇区
        addr = 0x1000 * j;
        //读扇区
        printf("Read data before write \r\n");
        memset(read_buf, 0, sizeof(read_buf));
        W25QXX_Read(read_buf, addr, sizeof(read_buf));
        printf("read data is:\r\n");
        for (i = 0; i < sizeof(read_buf); ++i) {
            printf("%02X ", read_buf[i]);
        }
        printf("\r\n");
        HAL_Delay(20);

        //擦除该扇区
        printf("Erase sector %lu\r\n", addr / 0x1000);
        W25QXX_Erase_Sector(addr);
        HAL_Delay(20);
        //写数据
        printf("Write data\r\n");
        W25QXX_Write(dat, addr, sizeof(dat));
        HAL_Delay(20);

        //再次读数据
        printf("Read data after write\r\n");
        memset(read_buf, 0, sizeof(read_buf));
        W25QXX_Read(read_buf, addr, sizeof(read_buf));
        printf("read data is:\r\n");
        for (i = 0; i < sizeof(read_buf); ++i) {
            printf("%02X ", read_buf[i]);
        }
        printf("\r\n");
        HAL_Delay(20);

        uint8_t chkok = 1;
        for (i = 0; i < sizeof(dat); ++i) {
            if (dat[i] != read_buf[i]) {
                chkok = 0;
                break;
            }
        }
        printf("sector %lu write %s!\r\n", addr / 0x1000, chkok ? "ok" : "error");
        if (!chkok) {
            break;
        }
    }
}

void fs_test(void) {
    BYTE work[4096]; // FATFS工作缓冲区
    FATFS fs;//文件系统对象
    FIL fil;//文件对象
    FRESULT res;//操作返回结果
    UINT i;//临时变量
    BYTE mm[500];// 临时数组

    res = f_mount(&fs, "0:", 1);
    if (res == 0X0D) { // FLASH磁盘,FAT文件系统错误，重新格式化FLASH
        printf("Flash Disk Formatting...\r\n");//格式化FLASH
        res = f_mkfs("0:", FM_ANY, 0, work, sizeof(work));
        if (res != FR_OK) {
            printf("mkfs error.\r\n");
        }
    }
    if (res == FR_OK) {
        printf("FATFS Init ok!\r\n");
    }

    res = f_open(&fil, "0:/test.txt", FA_CREATE_NEW);
    if (res != FR_OK && res != FR_EXIST) {
        printf("create file error.\r\n");
    }
    if (res == FR_EXIST) {
        res = f_open(&fil, "0:/test.txt", FA_WRITE | FA_READ | FA_OPEN_APPEND);
    }
    if (res != FR_OK) {
        printf("open file error.\r\n");
    } else {
        printf("open file ok.\r\n");
        f_puts("Hello, World!\r\n你好世界\r\n", &fil);
        printf("file size:%lu Bytes.\r\n", f_size(&fil));

        memset(mm, 0x0, 500);
        f_lseek(&fil, 0);
        res = f_read(&fil, mm, 500, &i);
        if (res == FR_OK) {
            printf("read size:%d Bytes.\r\n%s", i, mm);
        } else {
            printf("read error!\r\n");
        }
        f_close(&fil);
    }
    /*卸载文件系统*/
    f_mount(0, "0:", 0);
}

void fs_Init(bool breset) {
    BYTE work[4096]; // FATFS工作缓冲区
    FATFS fs;//文件系统对象
    FIL fil;//文件对象
    FRESULT res;//操作返回结果

    res = f_mount(&fs, "0:", 1);
    if (res == 0X0D) { // FLASH磁盘,FAT文件系统错误，重新格式化FLASH
        printf("Flash Disk Formatting...\r\n");//格式化FLASH
        res = f_mkfs("0:", FM_ANY, 0, work, sizeof(work));
        if (res != FR_OK) {
            printf("mkfs error.\r\n");
        }
    }
    if (res == FR_OK) {
        printf("FATFS Init ok!\r\n");
    }

    if (breset) {
        res = f_open(&fil, "0:/alarm.txt", FA_CREATE_ALWAYS);
    } else {
        res = f_open(&fil, "0:/alarm.txt", FA_CREATE_NEW);
    }
    if (res != FR_OK && res != FR_EXIST) {
        printf("create file error.\r\n");
    } else {
        printf("create alarm.txt ok\r\n");
        f_close(&fil);
    }

    if (breset) {
        res = f_open(&fil, "0:/para.txt", FA_CREATE_ALWAYS);
    } else {
        res = f_open(&fil, "0:/para.txt", FA_CREATE_NEW);
    }
    if (res != FR_OK && res != FR_EXIST) {
        printf("create file error.\r\n");
    } else {
        printf("create para.txt ok\r\n");
        f_close(&fil);
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
    HAL_Delay(1000);
#if testW25Q128
    TestW25Q128();
#endif
#if testFATFS
    fs_test();
    HAL_Delay(10000);
#endif
    fs_Init(false);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
