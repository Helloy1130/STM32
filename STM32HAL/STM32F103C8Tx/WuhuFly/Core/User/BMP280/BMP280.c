//
// Created by Y on 2020/10/27.
//

#include "BMP280.h"
#include "i2c.h"
#include <stdio.h>

static int mTempCalib[3];
static int mPressCalib[9];

int32_t t_fine;

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
    //read the press calibration data
    mPressCalib[0] = bmp280_read_word(BMP280_CALIB_P1_REG) & 0xffff;
    mPressCalib[1] = bmp280_read_word(BMP280_CALIB_P2_REG) & 0xffff;
    mPressCalib[2] = bmp280_read_word(BMP280_CALIB_P3_REG) & 0xffff;
    mPressCalib[3] = bmp280_read_word(BMP280_CALIB_P4_REG) & 0xffff;
    mPressCalib[4] = bmp280_read_word(BMP280_CALIB_P5_REG) & 0xffff;
    mPressCalib[5] = bmp280_read_word(BMP280_CALIB_P6_REG) & 0xffff;
    mPressCalib[6] = bmp280_read_word(BMP280_CALIB_P7_REG) & 0xffff;
    mPressCalib[7] = bmp280_read_word(BMP280_CALIB_P8_REG) & 0xffff;
    mPressCalib[8] = bmp280_read_word(BMP280_CALIB_P9_REG) & 0xffff;

    bmp280_write_byte(BMP280_CTRL_REG, BMP280_CTRL);

}

float readTemperature(void)
{
    int var1, var2;
    unsigned short dig_T1 = mTempCalib[0];
    short dig_T2 = mTempCalib[1];
    short dig_T3 = mTempCalib[2];

    uint8_t buffer[3];
    bmp280_read_bytes(BMP280_TEMP_REG, buffer, 3);
    int msb = buffer[0] & 0xff;
    int lsb = buffer[1] & 0xff;
    int xlsb = buffer[2] & 0xf0;
    int adc_T = (msb << 16 | lsb << 8 | xlsb) >> 4;

    var1  = ((((adc_T>>3) - ((int32_t)dig_T1 <<1))) *
             ((int32_t)dig_T2)) >> 11;

    var2  = (((((adc_T>>4) - ((int32_t)dig_T1)) *
               ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) *
             ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;

    float T  = (t_fine * 5 + 128) >> 8;
    return T/100;
}

float readPressure(void)
{
    int64_t var1, var2, p;
    unsigned short dig_P1 = mPressCalib[0];
    short dig_P2 = mPressCalib[1];
    short dig_P3 = mPressCalib[2];
    short dig_P4 = mPressCalib[3];
    short dig_P5 = mPressCalib[4];
    short dig_P6 = mPressCalib[5];
    short dig_P7 = mPressCalib[6];
    short dig_P8 = mPressCalib[7];
    short dig_P9 = mPressCalib[8];

    uint8_t buffer[3];
    bmp280_read_bytes(BMP280_PRESS_REG, buffer, 3);
    int msb = buffer[0] & 0xff;
    int lsb = buffer[1] & 0xff;
    int xlsb = buffer[2] & 0xf0;
    int adc_P = (msb << 16 | lsb << 8 | xlsb) >> 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
    var2 = var2 + (((int64_t)dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) +
           ((var1 * (int64_t)dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;

    if (var1 == 0) {
        return 0;  // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
    return (float)p/256;
}
