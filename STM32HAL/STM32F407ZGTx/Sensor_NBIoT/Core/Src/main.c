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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <retarget.h>
#include <stdio.h>
#include <math.h>
#include "gps.h"
#include "wh-nb73.h"
#include "INA226.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PI 3.14159265
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/*-------------------Sensor-------------------*/
uint8_t soil_temperature_humidity_ask[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b};
uint8_t soil_temperature_humidity_answer[9];
float soil_temperature, soil_humidity;

uint8_t wind_direction_ask[8] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x38};
uint8_t wind_direction_answer[9];
float wind_direction;

uint8_t wind_speed_ask[8] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x01, 0x85, 0xe8};
uint8_t wind_speed_answer[7];
float wind_speed;

uint8_t air_multi_element_ask[8] = {0x04, 0x03, 0x01, 0xf4, 0x00, 0x08, 0x04, 0x57};
uint8_t air_multi_element_answer[21];
float air_temperature, air_humidity, air_noise;
short  air_pressure, air_pm2_5, air_pm10;
int air_light;

uint8_t air_quality_ask[8] = {0x0A, 0x03, 0x00, 0x05, 0x00, 0x02, 0xD5, 0x71};
uint8_t air_quality_answer[9];
uint16_t air_CO2, air_CO;

uint8_t air_quality_ask2[8] = {0x0A, 0x03, 0x00, 0x0A, 0x00, 0x01, 0xA5, 0x73};
uint8_t air_quality_answer2[7];
float air_O2;

/*-------------------GPS-------------------*/
uint8_t USART1_TX_BUF[USART3_MAX_RECV_LEN];//串口1,发送缓存区
nmea_msg gpsx;//GPS信息
uint8_t dtbuf[50];//打印缓存器
const char *fixmode_tbl[4] = {"Fail","Fail"," 2D "," 3D "};	//fix mode字符串

/*-------------------Wind Sensor-------------------*/
uint16_t wind_direction_raw[500];
uint16_t wind_speed_raw[500];
int wind_data_num;
uint16_t wind_direction_average;
uint16_t wind_speed_average;
uint16_t wind_direction_max;
uint16_t wind_speed_max = 0;

/*-------------------Power Source Sensor-------------------*/
float bus_voltage_raw;
float bus_power_raw;
float shunt_voltage_raw;
float shunt_current_raw;
int power_data_num;
float bus_power_raw_buf[200];
uint32_t bus_voltage;
uint16_t shunt_current;
int16_t bus_power;
int16_t bus_power_average;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void readSensor(void)
{
    uint16_t crc_data;
    uint8_t data = 0;
    printf("-------------------Sensor-------------------\r\n");
    HAL_Delay(200);
    //Sensor - soil_temperature_humidity
    HAL_UART_Transmit(&huart2, soil_temperature_humidity_ask, 8, 100);
    while (1){
        HAL_UART_Receive(&huart2, &data,1,100);
        while (data != 0x01) {
            if (HAL_UART_Receive(&huart2, &data,1,100) != HAL_OK){
                break;
            }
        }
        if (data == 0x01) {
            soil_temperature_humidity_answer[0] = 0x01;
            HAL_UART_Receive(&huart2,soil_temperature_humidity_answer + 1,8,100);
        }
        if (soil_temperature_humidity_answer[0] == 0x01){
            crc_data = (uint16_t)(soil_temperature_humidity_answer[8] << 8) + soil_temperature_humidity_answer[7];
            if(crc16(soil_temperature_humidity_answer, 7) == crc_data){
                break;
            }
        }
        printf("Sensor - soil_temperature_humidity Read Error!\r\n");
        HAL_Delay(200);
        HAL_UART_Transmit(&huart2, soil_temperature_humidity_ask, 8, 100);
    }
    soil_humidity = (((uint16_t)soil_temperature_humidity_answer[3] << 8) + soil_temperature_humidity_answer[4])/10.0;
    soil_temperature = (((int16_t)soil_temperature_humidity_answer[5] << 8) + soil_temperature_humidity_answer[6])/10.0;
    printf("soil_humidity = %.1f%% RH, soil_temperature = %.1f C\r\n", soil_humidity, soil_temperature);

    HAL_Delay(200);
    //Sensor - air_multi_element
    HAL_UART_Transmit(&huart2, air_multi_element_ask, 8, 100);
    while (1){
        HAL_UART_Receive(&huart2, &data,1,100);
        while (data != 0x04) {
            if (HAL_UART_Receive(&huart2, &data,1,100) != HAL_OK){
                break;
            }
        }
        if (data == 0x04) {
            air_multi_element_answer[0] = 0x04;
            HAL_UART_Receive(&huart2,air_multi_element_answer + 1,20,100);
        }
        if (air_multi_element_answer[0] == 0x04){
            crc_data = (uint16_t)(air_multi_element_answer[20] << 8) + air_multi_element_answer[19];
            if(crc16(air_multi_element_answer, 19) == crc_data){
                break;
            }
        }
        printf("Sensor - air_multi_element Read Error!\r\n");
        HAL_UART_Transmit(&huart2, air_multi_element_ask, 8, 100);
    }
    air_humidity = (((uint16_t)air_multi_element_answer[3] << 8) + air_multi_element_answer[4])/10.0;
    air_temperature = (((int16_t)air_multi_element_answer[5] << 8) + air_multi_element_answer[6])/10.0;
    air_noise = (((uint16_t)air_multi_element_answer[7] << 8) + air_multi_element_answer[8])/10.0;
    air_pm2_5 = ((uint16_t)air_multi_element_answer[9] << 8) + air_multi_element_answer[10];
    air_pm10 = ((uint16_t)air_multi_element_answer[11] << 8) + air_multi_element_answer[12];
    air_pressure = ((uint16_t)air_multi_element_answer[13] << 8) + air_multi_element_answer[14];
    air_light = ((uint32_t)air_multi_element_answer[15] << 24) + ((uint32_t)air_multi_element_answer[16] << 16) +
                ((uint32_t)air_multi_element_answer[17] << 8) + air_multi_element_answer[18];
    printf("air_humidity = %.1f%% RH, air_temperature = %.1f C\r\n", air_humidity, air_temperature);
    printf("air_pm2.5 = %d ug/m^3, air_pm10 = %d ug/m^3\r\n", air_pm2_5, air_pm10);
    printf("air_pressure = %d hpa, air_light = %d Lux\r\n", air_pressure, air_light);

    HAL_Delay(200);
    //Sensor - air_quality
    HAL_UART_Transmit(&huart2, air_quality_ask, 8, 100);
    while (1){
        HAL_UART_Receive(&huart2, &data,1,100);
        while (data != 0x0A) {
            if (HAL_UART_Receive(&huart2, &data,1,100) != HAL_OK){
                break;
            }
        }
        if (data == 0x0A) {
            air_quality_answer[0] = 0x0A;
            HAL_UART_Receive(&huart2,air_quality_answer + 1,8,100);
        }
        if (air_quality_answer[0] == 0x0A){
            crc_data = (uint16_t)(air_quality_answer[8] << 8) + air_quality_answer[7];
            if(crc16(air_quality_answer, 7) == crc_data){
                break;
            }
        }
        printf("Sensor - air_quality Read Error!\r\n");
        HAL_UART_Transmit(&huart2, air_quality_ask, 8, 100);
    }
    air_CO2 = ((uint16_t)air_quality_answer[3] << 8) + air_quality_answer[4];
    air_CO = ((uint16_t)air_quality_answer[5] << 8) + air_quality_answer[6];
    printf("air_CO2 = %d ppm, air_CO = %d ppm\r\n", air_CO2, air_CO);

    HAL_Delay(200);
    HAL_UART_Transmit(&huart2, air_quality_ask2, 8, 100);
    while (1){
        HAL_UART_Receive(&huart2, &data,1,100);
        while (data != 0x0A) {
            if (HAL_UART_Receive(&huart2, &data,1,100) != HAL_OK){
                break;
            }
        }
        if (data == 0x0A) {
            air_quality_answer2[0] = 0x0A;
            HAL_UART_Receive(&huart2,air_quality_answer2 + 1,6,100);
        }
        if (air_quality_answer2[0] == 0x0A){
            crc_data = (uint16_t)(air_quality_answer2[6] << 8) + air_quality_answer2[5];
            if(crc16(air_quality_answer2, 5) == crc_data){
                break;
            }
        }
        printf("Sensor - air O2 Read Error!\r\n");
        HAL_UART_Transmit(&huart2, air_quality_ask2, 8, 100);
    }
    air_O2 = (((uint16_t)air_quality_answer2[3] << 8) + air_quality_answer2[4]) / 10.0;
    printf("air_O2 = %.1f %%\r\n", air_O2);
}

void readWindSensor(void)
{
    uint16_t crc_data;
    uint8_t data = 0;
    printf("-------------------Sensor-------------------\r\n");
    HAL_Delay(200);
    //Sensor - wind_direction
    HAL_UART_Transmit(&huart2, wind_direction_ask, 8, 100);
    while (1){
        HAL_UART_Receive(&huart2, &data,1,100);
        while (data != 0x02) {
            if (HAL_UART_Receive(&huart2, &data,1,100) != HAL_OK){
                break;
            }
        }
        if (data == 0x02) {
            wind_direction_answer[0] = 0x02;
            HAL_UART_Receive(&huart2,wind_direction_answer + 1,8,100);
        }
        if (wind_direction_answer[0] == 0x02){
            crc_data = (uint16_t)(wind_direction_answer[8] << 8) + wind_direction_answer[7];
            if(crc16(wind_direction_answer, 7) == crc_data){
                break;
            }
        }
        printf("Sensor - wind_direction Read Error!\r\n");
        HAL_UART_Transmit(&huart2, wind_direction_ask, 8, 100);
    }
    wind_direction = (((uint16_t)wind_direction_answer[3] << 8) + wind_direction_answer[4])/10.0;
    printf("wind_direction = %.1f degree\r\n", wind_direction);

    HAL_Delay(200);
    //Sensor - wind_speed
    HAL_UART_Transmit(&huart2, wind_speed_ask, 8, 100);
    while (1){
        HAL_UART_Receive(&huart2, &data,1,100);
        while (data != 0x03) {
            if (HAL_UART_Receive(&huart2, &data,1,100) != HAL_OK){
                break;
            }
        }
        if (data == 0x03) {
            wind_speed_answer[0] = 0x03;
            HAL_UART_Receive(&huart2,wind_speed_answer + 1,6,100);
        }
        if (wind_speed_answer[0] == 0x03){
            crc_data = (uint16_t)(wind_speed_answer[6] << 8) + wind_speed_answer[5];
            if(crc16(wind_speed_answer, 5) == crc_data){
                break;
            }
        }
        printf("Sensor - wind_speed Read Error!\r\n");
        HAL_UART_Transmit(&huart2, wind_speed_ask, 8, 100);
    }
    wind_speed = (((uint16_t)wind_speed_answer[3] << 8) + wind_speed_answer[4])/10.0;
    printf("wind_speed = %.1f m/s\r\n", wind_speed);
}

void windSensorProcess(void)
{
    wind_direction_raw[wind_data_num] = ((uint16_t)wind_direction_answer[3] << 8) + wind_direction_answer[4];
    wind_speed_raw[wind_data_num] = ((uint16_t)wind_speed_answer[3] << 8) + wind_speed_answer[4];
    wind_data_num++;
    if (wind_data_num == 200){
        int wind_speed_sum = 0, wind_max_index = 0;
        double wind_direction_u = 0, wind_direction_v = 0;
        wind_speed_max = 0;
        for (int i = 0; i < 200; ++i) {
            wind_direction_u += sin((wind_direction_raw[i] / 10.0) * PI / 180);
            wind_direction_v += cos((wind_direction_raw[i] / 10.0) * PI / 180);
            wind_speed_sum += wind_speed_raw[i];
            if(wind_speed_raw[i] > wind_speed_max){
                wind_max_index = i;
                wind_speed_max = wind_speed_raw[i];
            }
        }
        if (wind_direction_u >= 0 && wind_direction_v > 0){
            wind_direction_average = (uint16_t)((atan(wind_direction_u / wind_direction_v) * 180 / PI) * 10);
        }
        else if (wind_direction_u >= 0 && wind_direction_v < 0){
            wind_direction_average = (uint16_t)((atan(wind_direction_u / wind_direction_v) * 180 / PI + 180) * 10);
        }
        else if (wind_direction_u < 0 && wind_direction_v < 0){
            wind_direction_average = (uint16_t)((atan(wind_direction_u / wind_direction_v) * 180 / PI + 180) * 10);
        }
        else if (wind_direction_u < 0 && wind_direction_v > 0){
            wind_direction_average = (uint16_t)((atan(wind_direction_u / wind_direction_v) * 180 / PI + 360) * 10);
        }
        else if (wind_direction_u >= 0 && wind_direction_v == 0){
            wind_direction_average = (uint16_t)(90 * 10);
        }
        else if (wind_direction_u < 0 && wind_direction_v == 0){
            wind_direction_average = (uint16_t)(270 * 10);
        }

        wind_speed_average = wind_speed_sum / 200;
        wind_direction_max = wind_direction_raw[wind_max_index];
        wind_data_num = 0;
    }
    printf("wind_direction_average = %.1f degree\r\n", wind_direction_average/10.0);
    printf("wind_speed_average = %.1f m/s\r\n", wind_speed_average/10.0);
    printf("wind_direction_max = %.1f degree\r\n", wind_direction_max/10.0);
    printf("wind_speed_max = %.1f m/s\r\n", wind_speed_max/10.0);
}

void Gps_Msg_Show(void)
{
    float tp;
    tp = gpsx.longitude;
    sprintf((char *)dtbuf, "Longitude:%.5f %1c   ", tp/100000, gpsx.ewhemi);	//得到经度字符串
    printf("%s\r\n",dtbuf);
    tp = gpsx.latitude;
    sprintf((char *)dtbuf, "Latitude:%.5f %1c   ", tp/100000, gpsx.nshemi);	//得到纬度字符串
    printf("%s\r\n", dtbuf);
    tp = gpsx.altitude;
    sprintf((char *)dtbuf, "Altitude:%.1fm     ", tp/10);	    			//得到高度字符串
    printf("%s\r\n", dtbuf);
    tp = gpsx.speed;
    sprintf((char *)dtbuf, "Speed:%.3fkm/h     ", tp/1000);		    		//得到速度字符串
    printf("%s\r\n", dtbuf);
    if(gpsx.fixmode <= 3)														//定位状态
    {
        sprintf((char *)dtbuf, "Fix Mode:%s", fixmode_tbl[gpsx.fixmode]);
        printf("%s\r\n", dtbuf);
    }
    sprintf((char *)dtbuf, "GPS+BD Valid satellite:%02d", gpsx.posslnum);	 	//用于定位的GPS卫星数
    printf("%s\r\n", dtbuf);
    sprintf((char *)dtbuf, " GPS Visible satellite:%02d", gpsx.svnum%100);	 	//可见GPS卫星数
    printf("%s\r\n", dtbuf);
    sprintf((char *)dtbuf, "BD Visible satellite:%02d", gpsx.beidou_svnum%100);//可见北斗卫星数
    printf("%s\r\n", dtbuf);
    sprintf((char *)dtbuf, "UTC Date:%04d/%02d/%02d   ", gpsx.utc.year, gpsx.utc.month, gpsx.utc.date);//显示UTC日期
    printf("%s\r\n", dtbuf);
    sprintf((char *)dtbuf, "UTC Time:%02d:%02d:%02d   ", gpsx.utc.hour, gpsx.utc.min, gpsx.utc.sec);//显示UTC时间
    printf("%s\r\n", dtbuf);
}

void readINA226(void)
{
    printf("Bus voltage:   ");
    bus_voltage_raw = INA226_readBusVoltage();
    printf("%5f", bus_voltage_raw);
    printf(" V\r\n");

//    printf("Bus power:     ");
//    bus_power_raw = INA226_readBusPower();
//    printf("%5f", bus_power_raw);
//    printf(" W\r\n");

    printf("Shunt voltage: ");
    shunt_voltage_raw = INA226_readShuntVoltage();
    printf("%5f", shunt_voltage_raw);
    printf(" V\r\n");

    printf("Shunt current: ");
//    shunt_current_raw = INA226_readShuntCurrent();
    shunt_current_raw = shunt_voltage_raw * 10;//100 mOm
    printf("%5f", shunt_current_raw);
    printf(" A\r\n");

    bus_voltage = (uint16_t)(bus_voltage_raw * 1000);
    shunt_current = (int16_t)(shunt_current_raw * 1000);
//    bus_power = (uint16_t)(bus_power_raw * 100);

    bus_power_raw_buf[power_data_num] = bus_voltage_raw * shunt_current_raw;
    bus_power = (int16_t)(bus_power_raw_buf[power_data_num] * 100);

    printf("Bus power:     ");
    printf("%5f", bus_power_raw_buf[power_data_num]);
    printf(" W\r\n");

    printf("\r\n");

    power_data_num++;

    if (power_data_num == 200) {
        power_data_num = 0;
        float bus_power_raw_average = 0;
        for (int i = 0; i < 200; ++i) {
            bus_power_raw_average += bus_power_raw_buf[i];
        }
        bus_power_raw_average = bus_power_raw_average / 200;
        bus_power_average = (int16_t)(bus_power_raw_average * 100);
    }
}

void nb73_send(void)
{
    uint16_t crc;

    uint8_t tmp[61] = {0x01, 0x46, 0x00, 0x00, 0x00, 26, 52, 0x00};

    /*soil_humidity=data/10*/
    tmp[7] = soil_temperature_humidity_answer[3];
    tmp[8] = soil_temperature_humidity_answer[4];
    /*soil_temperature=data/10*/
    tmp[9] = soil_temperature_humidity_answer[5];
    tmp[10] = soil_temperature_humidity_answer[6];
    /*wind_direction=data/10*/
    tmp[11] = (uint8_t)(wind_direction_average>>8);
    tmp[12] = (uint8_t)(wind_direction_average);
    /*wind_speed=data/10*/
    tmp[13] = (uint8_t)(wind_speed_average>>8);
    tmp[14] = (uint8_t)(wind_speed_average);
    /*air_humidity=data/10*/
    tmp[15] = air_multi_element_answer[3];
    tmp[16] = air_multi_element_answer[4];
    /*air_temperature=data/10*/
    tmp[17] = air_multi_element_answer[5];
    tmp[18] = air_multi_element_answer[6];
    /*air_pm2.5=data*/
    tmp[19] = air_multi_element_answer[9];
    tmp[20] = air_multi_element_answer[10];
    /*air_pm10=data*/
    tmp[21] = air_multi_element_answer[11];
    tmp[22] = air_multi_element_answer[12];
    /*air_pressure=data/10*/
    tmp[23] = air_multi_element_answer[13];
    tmp[24] = air_multi_element_answer[14];
    /*air_light=data*/
    tmp[25] = air_multi_element_answer[15];
    tmp[26] = air_multi_element_answer[16];
    tmp[27] = air_multi_element_answer[17];
    tmp[28] = air_multi_element_answer[18];
    /*air_CO2=data*/
    tmp[29] = air_quality_answer[3];
    tmp[30] = air_quality_answer[4];
    /*air_CO=data*/
    tmp[31] = air_quality_answer[5];
    tmp[32] = air_quality_answer[6];
    /*air_O2=data*/
    tmp[33] = air_quality_answer2[3];
    tmp[34] = air_quality_answer2[4];
    /*gps_Longitude=data/100000*/
    tmp[35] = (uint8_t)(gpsx.longitude >> 24);
    tmp[36] = (uint8_t)(gpsx.longitude >> 16);
    tmp[37] = (uint8_t)(gpsx.longitude >> 8);
    tmp[38] = (uint8_t)(gpsx.longitude);
    /*gps_ewhemi=data*/
    tmp[39] = 0;
    if (gpsx.ewhemi == 'E') {
        tmp[40] = 1;
    }
    else if (gpsx.ewhemi == 'W') {
        tmp[40] = 2;
    }
    else {
        tmp[40] = 0;
    }
    /*gps_Latitude=data/100000*/
    tmp[41] = (uint8_t)(gpsx.latitude >> 24);
    tmp[42] = (uint8_t)(gpsx.latitude >> 16);
    tmp[43] = (uint8_t)(gpsx.latitude >> 8);
    tmp[44] = (uint8_t)(gpsx.latitude);
    /*gps_nshemi=data*/
    tmp[45] = 0;
    if (gpsx.nshemi == 'N') {
        tmp[46] = 1;
    }
    else if (gpsx.nshemi == 'S') {
        tmp[46] = 2;
    }
    else {
        tmp[46] = 0;
    }
    /*wind_direction=data/10*/
    tmp[47] = (uint8_t)(wind_direction_max>>8);
    tmp[48] = (uint8_t)(wind_direction_max);
    /*wind_speed=data/10*/
    tmp[49] = (uint8_t)(wind_speed_max>>8);
    tmp[50] = (uint8_t)(wind_speed_max);
    /*bus voltage=data/1000*/
    tmp[51] = (uint8_t)(bus_voltage>>8);
    tmp[52] = (uint8_t)(bus_voltage);
    /*shunt current=data/1000*/
    tmp[53] = (uint8_t)(shunt_current>>8);
    tmp[54] = (uint8_t)(shunt_current);
    /*bus power average=data/100*/
    tmp[55] = (uint8_t)(bus_power_average>>8);
    tmp[56] = (uint8_t)(bus_power_average);
    /*bus power=data/100*/
    tmp[57] = (uint8_t)(bus_power>>8);
    tmp[58] = (uint8_t)(bus_power);

    crc = crc16(tmp, sizeof(tmp) - 2);

    tmp[59] = crc & 0xff;
    tmp[60] = crc >> 8;

    HAL_UART_Transmit(&huart6, tmp, sizeof(tmp), 200);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    uint16_t i,rxlen;
    uint8_t key;
    bool read_flag = false;
    uint8_t send_flag = 0;
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
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
    RetargetInit(&huart1);

    HAL_Delay(5000);
    USART3_RX_STA = 0;//清零
    HAL_UART_Receive_IT(&huart3, &rx3_data, 1); // 开启第一次接收中断
    if(SkyTra_Cfg_Rate(5) != 0)	//设置定位信息更新速度为5Hz,顺便判断GPS模块是否在位.
    {
        printf("SkyTraF8-BD Init...\r\n");
        do{
            SkyTra_Cfg_Prt(3);//设置模块的波特率为38400
            key = SkyTra_Cfg_Tp(100000);//脉冲宽度为100ms
        }while(SkyTra_Cfg_Rate(5)!=0 && key!=0);//配置SkyTraF8-BD的更新速率为5Hz
        printf("SkyTraF8-BD Init Finish\r\n");
    }

    printf("Initialize INA226\r\n");
    printf("-----------------------------------------------\r\n");
    // Configure INA226
    INA226_configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);

    // Calibrate INA226. Rshunt = 0.01 ohm, Max excepted current = 4A
    INA226_calibrate(0.01, 4);

    // Display configuration
    INA226_checkConfig();
    printf("-----------------------------------------------\r\n");

    HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      /*-------------------GPS-------------------*/
      printf("-------------------GPS-------------------\r\n");
      if(USART3_RX_STA & 0X8000)		//接收到一次数据了
      {
          rxlen = USART3_RX_STA & 0X7FFF;	//得到数据长度
          for(i = 0; i < rxlen; i++) {
              USART1_TX_BUF[i] = USART3_RX_BUF[i];
          }
          USART3_RX_STA = 0;		   	//启动下一次接收
          USART1_TX_BUF[i] = 0;			//自动添加结束符

          GPS_Analysis(&gpsx, (uint8_t *)USART1_TX_BUF);//分析字符串
          Gps_Msg_Show();//显示信息
          printf("\r\n");
      }

      readWindSensor();
      windSensorProcess();
      readINA226();

      if (gpsx.utc.min % 10 == 0 && read_flag == true){
          send_flag++;
          if (send_flag >= 2){
              send_flag = 0;
              readSensor();
              nb73_send();
              read_flag = false;
          }
      }
      if (gpsx.utc.min % 10 == 9 && read_flag == false){
          read_flag = true;
      }

      HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
      HAL_Delay(100);
      HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

      HAL_Delay(1500);
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
