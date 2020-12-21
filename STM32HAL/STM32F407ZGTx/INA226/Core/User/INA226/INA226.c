//
// Created by Y on 2020/11/27.
//

#include <stdio.h>
#include <math.h>
#include "INA226.h"
#include "i2c.h"

float currentLSB, powerLSB;
float vShuntMax, vBusMax, rShunt;

void setMaskEnable(uint16_t mask);
uint16_t getMaskEnable(void);


static int16_t bmp280_read_word(uint8_t reg)
{
    HAL_StatusTypeDef status;
    uint8_t byte_data[2];

    status = HAL_I2C_Mem_Read(&hi2c1, INA226_ADDRESS | (0x01),
                              reg, I2C_MEMADD_SIZE_8BIT, byte_data,
                              2, 100);
    if(status != HAL_OK){
        printf("%s: failed to read register\r\n", __func__);
        return -1;
    }
    return (byte_data[0] << 8 | byte_data[1]);
}

static int INA226_write_word(uint8_t reg, uint16_t data)
{
    HAL_StatusTypeDef status;
    uint8_t byte_data[2];
    byte_data[0] = (uint8_t)((data >> 8) & 0xff);
    byte_data[1] = (uint8_t)(data & 0xff);

    status = HAL_I2C_Mem_Write(&hi2c1, INA226_ADDRESS | (0x00),
                               reg, I2C_MEMADD_SIZE_8BIT, byte_data,
                               2, 100);
    if(status != HAL_OK){
        printf("%s: failed to write register\r\n", __func__);
        return -1;
    }
    return 0;
}

bool INA226_configure(ina226_averages_t avg, ina226_busConvTime_t busConvTime, ina226_shuntConvTime_t shuntConvTime, ina226_mode_t mode)
{
    uint16_t config = 0;

    config |= (avg << 9 | busConvTime << 6 | shuntConvTime << 3 | mode);

    vBusMax = 36;
    vShuntMax = 0.08192f;

    INA226_write_word(INA226_REG_CONFIG, config);

    return true;
}

bool INA226_calibrate(float rShuntValue, float iMaxExpected)
{
    uint16_t calibrationValue;
    rShunt = rShuntValue;

    float iMaxPossible, minimumLSB;

    iMaxPossible = vShuntMax / rShunt;

    minimumLSB = iMaxExpected / 32767;

    currentLSB = (uint16_t)(minimumLSB * 100000000);
    currentLSB /= 100000000;
    currentLSB /= 0.0001;
    currentLSB = ceil(currentLSB);
    currentLSB *= 0.0001;

    powerLSB = currentLSB * 25;

    calibrationValue = (uint16_t)((0.00512) / (currentLSB * rShunt));

    INA226_write_word(INA226_REG_CALIBRATION, calibrationValue);

    return true;
}

float INA226_getMaxPossibleCurrent(void)
{
    return (vShuntMax / rShunt);
}

float INA226_getMaxCurrent(void)
{
    float maxCurrent = (currentLSB * 32767);
    float maxPossible = INA226_getMaxPossibleCurrent();

    if (maxCurrent > maxPossible)
    {
        return maxPossible;
    } else
    {
        return maxCurrent;
    }
}

float INA226_getMaxShuntVoltage(void)
{
    float maxVoltage = INA226_getMaxCurrent() * rShunt;

    if (maxVoltage >= vShuntMax)
    {
        return vShuntMax;
    } else
    {
        return maxVoltage;
    }
}

float INA226_getMaxPower(void)
{
    return (INA226_getMaxCurrent() * vBusMax);
}

float INA226_readBusPower(void)
{
    return (bmp280_read_word(INA226_REG_POWER) * powerLSB);
}

float INA226_readShuntCurrent(void)
{
    return (bmp280_read_word(INA226_REG_CURRENT) * currentLSB);
}

float INA226_readShuntVoltage(void)
{
    float voltage;

    voltage = bmp280_read_word(INA226_REG_SHUNTVOLTAGE);

    return (voltage * 0.0000025);
}

float INA226_readBusVoltage(void)
{
    int16_t voltage;

    voltage = bmp280_read_word(INA226_REG_BUSVOLTAGE);

    return (voltage * 0.00125);
}

ina226_averages_t INA226_getAverages(void)
{
    uint16_t value;

    value = bmp280_read_word(INA226_REG_CONFIG);
    value &= 0b0000111000000000;
    value >>= 9;

    return (ina226_averages_t)value;
}

ina226_busConvTime_t INA226_getBusConversionTime(void)
{
    uint16_t value;

    value = bmp280_read_word(INA226_REG_CONFIG);
    value &= 0b0000000111000000;
    value >>= 6;

    return (ina226_busConvTime_t)value;
}

ina226_shuntConvTime_t INA226_getShuntConversionTime(void)
{
    uint16_t value;

    value = bmp280_read_word(INA226_REG_CONFIG);
    value &= 0b0000000000111000;
    value >>= 3;

    return (ina226_shuntConvTime_t)value;
}

ina226_mode_t INA226_getMode(void)
{
    uint16_t value;

    value = bmp280_read_word(INA226_REG_CONFIG);
    value &= 0b0000000000000111;

    return (ina226_mode_t)value;
}

void INA226_setMaskEnable(uint16_t mask)
{
    INA226_write_word(INA226_REG_MASKENABLE, mask);
}

uint16_t INA226_getMaskEnable(void)
{
    return bmp280_read_word(INA226_REG_MASKENABLE);
}

void INA226_enableShuntOverLimitAlert(void)
{
    INA226_write_word(INA226_REG_MASKENABLE, INA226_BIT_SOL);
}

void INA226_enableShuntUnderLimitAlert(void)
{
    INA226_write_word(INA226_REG_MASKENABLE, INA226_BIT_SUL);
}

void INA226_enableBusOvertLimitAlert(void)
{
    INA226_write_word(INA226_REG_MASKENABLE, INA226_BIT_BOL);
}

void INA226_enableBusUnderLimitAlert(void)
{
    INA226_write_word(INA226_REG_MASKENABLE, INA226_BIT_BUL);
}

void INA226_enableOverPowerLimitAlert(void)
{
    INA226_write_word(INA226_REG_MASKENABLE, INA226_BIT_POL);
}

void INA226_enableConversionReadyAlert(void)
{
    INA226_write_word(INA226_REG_MASKENABLE, INA226_BIT_CNVR);
}

void INA226_setBusVoltageLimit(float voltage)
{
    uint16_t value = voltage / 0.00125;
    INA226_write_word(INA226_REG_ALERTLIMIT, value);
}

void INA226_setShuntVoltageLimit(float voltage)
{
    uint16_t value = voltage * 25000;
    INA226_write_word(INA226_REG_ALERTLIMIT, value);
}

void INA226_setPowerLimit(float watts)
{
    uint16_t value = watts / powerLSB;
    INA226_write_word(INA226_REG_ALERTLIMIT, value);
}

void INA226_setAlertInvertedPolarity(bool inverted)
{
    uint16_t temp = getMaskEnable();

    if (inverted)
    {
        temp |= INA226_BIT_APOL;
    } else
    {
        temp &= ~INA226_BIT_APOL;
    }

    setMaskEnable(temp);
}

void INA226_setAlertLatch(bool latch)
{
    uint16_t temp = getMaskEnable();

    if (latch)
    {
        temp |= INA226_BIT_LEN;
    } else
    {
        temp &= ~INA226_BIT_LEN;
    }

    setMaskEnable(temp);
}

bool INA226_isMathOverflow(void)
{
    return ((getMaskEnable() & INA226_BIT_OVF) == INA226_BIT_OVF);
}

bool INA226_isAlert(void)
{
    return ((getMaskEnable() & INA226_BIT_AFF) == INA226_BIT_AFF);
}

void INA226_checkConfig(void)
{
    printf("Mode:                  ");
    switch (INA226_getMode())
    {
        case INA226_MODE_POWER_DOWN:      printf("Power-Down\r\n"); break;
        case INA226_MODE_SHUNT_TRIG:      printf("Shunt Voltage, Triggered\r\n"); break;
        case INA226_MODE_BUS_TRIG:        printf("Bus Voltage, Triggered\r\n"); break;
        case INA226_MODE_SHUNT_BUS_TRIG:  printf("Shunt and Bus, Triggered\r\n"); break;
        case INA226_MODE_ADC_OFF:         printf("ADC Off\r\n"); break;
        case INA226_MODE_SHUNT_CONT:      printf("Shunt Voltage, Continuous\r\n"); break;
        case INA226_MODE_BUS_CONT:        printf("Bus Voltage, Continuous\r\n"); break;
        case INA226_MODE_SHUNT_BUS_CONT:  printf("Shunt and Bus, Continuous\r\n"); break;
        default: printf("unknown\r\n");
    }

    printf("Samples average:       ");
    switch (INA226_getAverages())
    {
        case INA226_AVERAGES_1:           printf("1 sample\r\n"); break;
        case INA226_AVERAGES_4:           printf("4 samples\r\n"); break;
        case INA226_AVERAGES_16:          printf("16 samples\r\n"); break;
        case INA226_AVERAGES_64:          printf("64 samples\r\n"); break;
        case INA226_AVERAGES_128:         printf("128 samples\r\n"); break;
        case INA226_AVERAGES_256:         printf("256 samples\r\n"); break;
        case INA226_AVERAGES_512:         printf("512 samples\r\n"); break;
        case INA226_AVERAGES_1024:        printf("1024 samples\r\n"); break;
        default: printf("unknown\r\n");
    }

    printf("Bus conversion time:   ");
    switch (INA226_getBusConversionTime())
    {
        case INA226_BUS_CONV_TIME_140US:  printf("140uS\r\n"); break;
        case INA226_BUS_CONV_TIME_204US:  printf("204uS\r\n"); break;
        case INA226_BUS_CONV_TIME_332US:  printf("332uS\r\n"); break;
        case INA226_BUS_CONV_TIME_588US:  printf("558uS\r\n"); break;
        case INA226_BUS_CONV_TIME_1100US: printf("1.100ms\r\n"); break;
        case INA226_BUS_CONV_TIME_2116US: printf("2.116ms\r\n"); break;
        case INA226_BUS_CONV_TIME_4156US: printf("4.156ms\r\n"); break;
        case INA226_BUS_CONV_TIME_8244US: printf("8.244ms\r\n"); break;
        default: printf("unknown\r\n");
    }

    printf("Shunt conversion time: ");
    switch (INA226_getShuntConversionTime())
    {
        case INA226_SHUNT_CONV_TIME_140US:  printf("140uS\r\n"); break;
        case INA226_SHUNT_CONV_TIME_204US:  printf("204uS\r\n"); break;
        case INA226_SHUNT_CONV_TIME_332US:  printf("332uS\r\n"); break;
        case INA226_SHUNT_CONV_TIME_588US:  printf("558uS\r\n"); break;
        case INA226_SHUNT_CONV_TIME_1100US: printf("1.100ms\r\n"); break;
        case INA226_SHUNT_CONV_TIME_2116US: printf("2.116ms\r\n"); break;
        case INA226_SHUNT_CONV_TIME_4156US: printf("4.156ms\r\n"); break;
        case INA226_SHUNT_CONV_TIME_8244US: printf("8.244ms\r\n"); break;
        default: printf("unknown\r\n");
    }

    printf("Max possible current:  ");
    printf("%f", INA226_getMaxPossibleCurrent());
    printf(" A\r\n");

    printf("Max current:           ");
    printf("%f", INA226_getMaxCurrent());
    printf(" A\r\n");

    printf("Max shunt voltage:     ");
    printf("%f", INA226_getMaxShuntVoltage());
    printf(" V\r\n");

    printf("Max power:             ");
    printf("%f", INA226_getMaxPower());
    printf(" W\r\n");
}
