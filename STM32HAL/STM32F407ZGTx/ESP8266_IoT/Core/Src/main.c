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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "sht30.h"
#include "ESP8266.h"
#include "ESP8266_MQTT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//WIFI configuration
#define WIFI_NAME     "OPPO R15 meng"
#define WIFI_PASSWD   "00000000"

//Ali IoT Sign up Configuration
#define MQTT_BROKERADDRESS              "a1BAujzY0ka.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define MQTT_BROKERPORT                 1883
#define MQTT_CLIENTID                   "1|securemode=3,signmethod=hmacsha1|"
#define MQTT_USARNAME                   "deviceSN2&a1BAujzY0ka"
#define MQTT_PASSWD                     "00714567B47DEF72E45FB1C1DFED077B196BDAB0"
#define	MQTT_PUBLISH_TOPIC              "/sys/a1BAujzY0ka/deviceSN2/thing/event/property/post"
#define MQTT_SUBSCRIBE_TOPIC            "/sys/a1BAujzY0ka/deviceSN2/thing/service/property/set"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
bool LightSwitch = false;
bool SmokeAlarm = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ESP8266_MQTT_Init(void)
{
    uint8_t status = 0;

    while(status < 5){
        //0.ESP8266 Init
        if(ESP8266_Init()){
            printf("ESP8266 Init OK!\r\n");
            status++;
        } else {
            printf("ESP8266 Init FAIL!\r\n");
        }
        //1.ESP8266 Connect AP
        if(status == 1){
            if(ESP8266_ConnectAP(WIFI_NAME, WIFI_PASSWD)){
                printf("ESP8266 Connect AP OK!\r\n");
                status++;
            } else {
                printf("ESP8266 Connect AP FAIL!\r\n");
            }
        }
        //2.connect Ali IoT Sever
        if(status == 2){
            if(ESP8266_ConnectServer("TCP", MQTT_BROKERADDRESS, MQTT_BROKERPORT) != 0){
                printf("ESP8266 Connect Server OK!\r\n");
                status++;
            } else {
                printf("ESP8266 Connect Server FAIL!\r\n");
            }
        }
        //3.MQTT Sign up AliCloud
        if(status == 3){
            if(MQTT_Connect(MQTT_CLIENTID, MQTT_USARNAME, MQTT_PASSWD) != 0){
                printf("ESP8266 Connect Aliyun OK!\r\n");
                status++;
            } else {
                printf("ESP8266 Connect Aliyun FAIL!\r\n");
            }
        }
        //4.Subscribe Topic
        if(status == 4){
            if(MQTT_SubscribeTopic(MQTT_SUBSCRIBE_TOPIC,0,1) != 0){
                printf("ESP8266 Subscribe Topic OK!\r\n");
                status++;
            } else {
                printf("ESP8266 Subscribe Topic FAIL!\r\n");
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
    float humidity;
    char mqtt_message[300];	//MQTT data buffer
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
    SHT_Init();

    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    ESP8266_MQTT_Init();

    HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      clear_rx_buf();
      sht30_data_process();
      temperature = data_process.SHT30_temperature / 10.0f;
      humidity = data_process.SHT30_humidity / 10.0f;
      sprintf(mqtt_message,
              "{"
              "\"method\":\"thing.event.property.post\","
              "\"id\":\"1\","
              "\"params\":{"
              "\"RoomTemp\":%.1f,"
              "\"RoomHumidity\":%.1f,"
              "\"LightSwitch\":%d,"
              "\"SmokeAlarm\":%d"
              "},"
              "\"version\":\"1.0\""
              "}",
              temperature, humidity, LightSwitch, SmokeAlarm);
//      printf("%s", mqtt_message);
      MQTT_PublishData(MQTT_PUBLISH_TOPIC, mqtt_message, 0);

      HAL_Delay(2000);

      if(rx_index > 0){
//          for (int i = 0; i < rx_index; ++i) {
//              printf("%c", rx_buf[i]);
//          }
          if(strstr((char *)(rx_buf+4), "\"LightSwitch\":1") != NULL) {
              printf("Open Light\r\n");
              LightSwitch = true;
          }
          else{
              printf("Close Light\r\n");
              LightSwitch = false;
          }
          clear_rx_buf();
      }

      if(LightSwitch == true){
          HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
      }
      else{
          HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
      }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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
  __disable_irq();
  while (1)
  {
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
