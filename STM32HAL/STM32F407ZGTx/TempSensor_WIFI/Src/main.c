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
#include <string.h>
#include <stdlib.h>
#include "esp8266_at.h"
#include "esp8266_mqtt.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//WIFI配置
#define WIFI_NAME     "OPPO R15 meng"
#define WIFI_PASSWD   "00000000"

//阿里云服务器的登陆配置
#define MQTT_BROKERADDRESS    "a1ZtK7S3DOC.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define MQTT_BROKERPORT       1883
#define MQTT_CLIENTID         "12345|securemode=3,signmethod=hmacsha1,timestamp=0|"
#define MQTT_USARNAME         "STM32F407_NODE1&a1ZtK7S3DOC"
#define MQTT_PASSWD           "EADACCBF85E81B6907CC2E139D57235F2DD09C24"
#define	MQTT_PUBLISH_TOPIC    "/sys/a1ZtK7S3DOC/STM32F407_NODE1/thing/event/property/post"
#define MQTT_SUBSCRIBE_TOPIC  "/sys/a1ZtK7S3DOC/STM32F407_NODE1/thing/event/property/post_reply"

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

//UART2接收数据中断的回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  HAL_StatusTypeDef status;

  rx_buf[rx_index++] = rx_byte;
  do {
    status = HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
  } while(status != HAL_OK); 
}

//MQTT初始化函数
void ESP8266_MQTT_Init(void)
{
  uint8_t status = 0;
  
  while(status < 5){
    //0.ESP8266初始化
    if(ESP8266_Init()){
      printf("ESP8266初始化成功！\r\n");
      status++;
    } else {
      printf("ESP8266初始化失败！\r\n");
    }
    //1.连接热点
    if(status == 1){
      if(ESP8266_ConnectAP(WIFI_NAME, WIFI_PASSWD)){
        printf("ESP8266连接热点成功！\r\n");
        status++;
      } else {
        printf("ESP8266连接热点失败！\r\n");
      }
    }
    //2.连接阿里云IOT服务器
    if(status == 2){
      if(ESP8266_ConnectServer("TCP", MQTT_BROKERADDRESS, MQTT_BROKERPORT) != 0){
        printf("ESP8266连接阿里云服务器成功！\r\n");
        status++;
      } else {
        printf("ESP8266连接阿里云服务器失败！\r\n");
      }
    }
    //3.MQTT登录阿里云
    if(status == 3){
      if(MQTT_Connect(MQTT_CLIENTID, MQTT_USARNAME, MQTT_PASSWD) != 0){
        printf("ESP8266阿里云MQTT登陆成功！\r\n");
        status++;
      } else {
        printf("ESP8266阿里云MQTT登陆失败！\r\n");
      }
    }
    //4.订阅主题
    if(status == 4){
      if(MQTT_SubscribeTopic(MQTT_SUBSCRIBE_TOPIC,0,1) != 0){
        printf("ESP8266阿里云MQTT订阅主题成功！\r\n");
        status++;
      } else {
        printf("ESP8266阿里云MQTT订阅主题失败！\r\n");
      }
    }
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
	float temperature;
  char mqtt_message[300];	//MQTT的上报消息缓存
  
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
  ESP8266_MQTT_Init();
  
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
    
    temperature = bmp280_get_temp();
    printf("temperature = %f\r\n", temperature);
    sprintf(mqtt_message,
            "{\"id\":%d,\
              \"version\":\"1.0\",\
              \"params\":{\"CurrentTemperature\":%.02f\
                          },\
              \"method\":\"thing.event.property.post\"}",
             123, temperature);
    MQTT_PublishData(MQTT_PUBLISH_TOPIC, mqtt_message, 0);
    HAL_Delay(10000);
    if(rx_index > 0){
      printf("%s\r\n", (char *)rx_buf);
      clear_rx_buf();
    }
    //printf("%s\r\n", mqtt_message);
    
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
