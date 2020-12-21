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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "bmp280.h"
#include <jansson.h>
#include <string.h>
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define RX_LEN        128

static uint8_t rx_byte;
static uint8_t rx_index = 0;
static uint8_t rx_buf[RX_LEN];
static uint8_t mqtt_connected = 0;

static void clear_rx_buf(void)
{
  rx_index = 0;
  memset(rx_buf, 0x00, RX_LEN);
}

//UART2接收数据中断的回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  HAL_StatusTypeDef status;
  /* Prevent unused argument(s) compilation warning */
  //UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
  //printf("%c", rx_byte);
  rx_buf[rx_index++] = rx_byte;
  do {
    status = HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
  } while(status != HAL_OK); 
}

static void wait_for_connected(void)
{
  printf("wait for mqtt connected...\r\n");
  while(!mqtt_connected){
    if(rx_index > 0 && (rx_buf[rx_index - 1] == 0x0d)){
      char *strx = strstr((const char *)rx_buf,
                    "CONNECT OK");
      if(strx) {
        printf("MQTT CONNECT OK!!!\r\n");
        mqtt_connected = 1;
      }
      clear_rx_buf();
    }
  }
}

static void publish(const char *topic, const char *msg, uint32_t len)
{
  uint8_t sendbuf1[128];
  uint8_t sendbuf2[128];
  uint8_t sendbytes = 0;
  
  memset(sendbuf1, 0x00, 128);
  memset(sendbuf2, 0x00, 128);
  sendbytes += snprintf((char *)sendbuf1, 128, "AT+PUBILSH=\"%s\",%d,0\r\n", topic, len);
  HAL_UART_Transmit(&huart2, sendbuf1, sendbytes, 0xFFFF);
  sendbytes += snprintf((char *)sendbuf2, 128, "%s\r\n", msg);
  HAL_Delay(100);
  HAL_UART_Transmit(&huart2, sendbuf2, sendbytes, 0xFFFF);
  HAL_Delay(100);
}

//json数据打印测试
void json_test(void)
{
  json_t *root;
  char *str;
  
  root = json_pack("{sisfss}",
          "id", 123,
          "temp", 23.8,
          "method", "post");
  str = json_dumps(root, JSON_ENCODE_ANY);
  if(str != NULL){
    printf("str: %s\r\n", str);
  } else {
    printf("failed to dumps root\r\n");
  }
  
  free(root);//释放内存
  free(str);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	float temperature;
  static char temp_str[16];
  static uint8_t bytes = 0;
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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  bmp280_init();
  HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
  //json_test();
  //AT指令集配置GPRS
//  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+MQTTMODE?\r\n", 14, 0xFFFF);
//  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+IPPORT=\"a139Uz564DX.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883\r\n", 67, 0xFFFF);
//  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CLIENTID=\"12345|securemode=3,signmethod=hmacsha1,timestamp=0|\"\r\n", 67, 0xFFFF);
//  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+USERPWD=\"STM32F407ZG_NODE001&a139Uz564DX\",\"A30B37003C802426E0B50B5281C30DBFEBC8D925\"\r\n", 89, 0xFFFF);
//  HAL_UART_Transmit(&huart2, (uint8_t *)"AT&W\r\n", 6, 0xFFFF);
//  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CFUN=1\r\n", 11, 0xFFFF);

  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+MQTTMODE?\r\n", 14, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+IPPORT=\"a139Uz564DX.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883\r\n", 67, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CLIENTID=\"12345|securemode=3,signmethod=hmacsha1,timestamp=0|\"\r\n", 67, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+USERPWD=\"STM32F407_NODE1&a139Uz564DX\",\"137C892C37178F42F8A9F32B8745C8A2F8A101F4\"\r\n", 85, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t *)"AT&W\r\n", 6, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CFUN=1\r\n", 11, 0xFFFF);
  
  wait_for_connected();//等待GPRS连上阿里云
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
		HAL_Delay(1000);
    //HAL_UART_Transmit(&huart2, (uint8_t *)"Hello", 5, 0xFFFF);
    
    if(rx_index > 0){
      printf("%s\r\n", (char *)rx_buf);
      clear_rx_buf();
    }
    
    temperature = bmp280_get_temp();
    printf("temperature = %f\r\n", temperature);
    memset(temp_str, 0, 16);
    bytes = snprintf(temp_str, 16, "%.02f", temperature);
    clear_rx_buf();
    publish("TEMP", temp_str, bytes);
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

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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
  /** Initializes the CPU, AHB and APB busses clocks 
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
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
