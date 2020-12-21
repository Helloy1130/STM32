#include "bmp280.h"
#include "i2c.h"
#include <stdio.h>

static int mTempCalib[3];

//连续读取多个数据
static int bmp280_read_bytes(uint8_t addr, uint8_t *data, uint8_t size)
{
  HAL_StatusTypeDef status;
	
	status = HAL_I2C_Mem_Read(&hi2c1, BMP280_SLAVE_ADDR, 
			addr, I2C_MEMADD_SIZE_8BIT, data, 
			size, 100);
	if(status != HAL_OK){
		printf("%s: failed to read register\r\n", __func__);
    return -1;
	}
  return 0;
}

static int bmp280_read_word(uint8_t addr)
{
  HAL_StatusTypeDef status;
	uint8_t byte_data[2];
	
	status = HAL_I2C_Mem_Read(&hi2c1, BMP280_SLAVE_ADDR, 
			addr, I2C_MEMADD_SIZE_8BIT, byte_data, 
			2, 100);
	if(status != HAL_OK){
		printf("%s: failed to read register\r\n", __func__);
    return -1;
	}
  return (byte_data[1] << 8 | byte_data[0]);
}

static int bmp280_read_byte(uint8_t addr)
{
  HAL_StatusTypeDef status;
	uint8_t byte_data;
	
	status = HAL_I2C_Mem_Read(&hi2c1, BMP280_SLAVE_ADDR, 
			addr, I2C_MEMADD_SIZE_8BIT, &byte_data, 
			1, 100);
	if(status != HAL_OK){
		printf("%s: failed to read register\r\n", __func__);
    return -1;
	}
  return byte_data;
}

static int bmp280_write_byte(uint8_t addr, uint8_t data)
{
  HAL_StatusTypeDef status;
	
	status = HAL_I2C_Mem_Write(&hi2c1, BMP280_SLAVE_ADDR, 
			addr, I2C_MEMADD_SIZE_8BIT, &data, 
			1, 100);
	if(status != HAL_OK){
		printf("%s: failed to write register\r\n", __func__);
    return -1;
	}
  return 0;
}

void bmp280_init(void)
{
	HAL_StatusTypeDef status;
	uint8_t chipid = 0;
	
	//read the sensor chip id
	status = HAL_I2C_Mem_Read(&hi2c1, BMP280_SLAVE_ADDR, 
			BMP280_CHIPID_REG, I2C_MEMADD_SIZE_8BIT, &chipid, 
			1, 100);
	if(status != HAL_OK){
		printf("%s: failed to read chip id\r\n", __func__);
    return;
	}
  printf("%s:chip id = 0x%02x\r\n", __func__, chipid);
  if(chipid != BMP280_CHIPID){
    printf("%s: chip id not match bmp280\r\n", __func__);
  }
  //read the sensor calibration data
  mTempCalib[0] = bmp280_read_word(BMP280_CALIB_T1_REG) & 0xffff;
  mTempCalib[1] = bmp280_read_word(BMP280_CALIB_T2_REG) & 0xffff;
  mTempCalib[2] = bmp280_read_word(BMP280_CALIB_T3_REG) & 0xffff;
  
  bmp280_write_byte(BMP280_CTRL_REG, BMP280_CTRL);
  
}

static float bmp280_cal_temp(int raw, int *calib)
{
  unsigned short dig_T1 = calib[0];
  short dig_T2 = calib[1];
	short dig_T3 = calib[2];
  
  float adc_T = (float) raw;
	float var1 = (adc_T / 16384.0f - ((float) dig_T1) / 1024.0f) * ((float) dig_T2);
	float var2 = ((adc_T / 131072.0f - ((float) dig_T1) / 8192.0f) * 
    (adc_T / 131072.0f - ((float) dig_T1) / 8192.0f)) * ((float) dig_T3);
  float fineTemp = var1 + var2;
  
	return fineTemp / 5120.0f;
}

float bmp280_get_temp(void)
{
  uint8_t buffer[3];
  bmp280_read_bytes(BMP280_TEMP_REG, buffer, 3);
  int msb = buffer[0] & 0xff;
  int lsb = buffer[1] & 0xff;
  int xlsb = buffer[2] & 0xf0;
  int raw = (msb << 16 | lsb << 8 | xlsb) >> 4;
  
  return bmp280_cal_temp(raw, mTempCalib);
}

