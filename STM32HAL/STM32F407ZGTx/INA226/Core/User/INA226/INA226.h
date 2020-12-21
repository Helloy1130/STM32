//
// Created by Y on 2020/11/27.
//
#ifndef INA226_INA226_H
#define INA226_INA226_H

#include <stdbool.h>
#include "main.h"

#define INA226_ADDRESS              (0x40 << 1)

#define INA226_REG_CONFIG           (0x00)
#define INA226_REG_SHUNTVOLTAGE     (0x01)
#define INA226_REG_BUSVOLTAGE       (0x02)
#define INA226_REG_POWER            (0x03)
#define INA226_REG_CURRENT          (0x04)
#define INA226_REG_CALIBRATION      (0x05)
#define INA226_REG_MASKENABLE       (0x06)
#define INA226_REG_ALERTLIMIT       (0x07)

#define INA226_BIT_SOL              (0x8000)
#define INA226_BIT_SUL              (0x4000)
#define INA226_BIT_BOL              (0x2000)
#define INA226_BIT_BUL              (0x1000)
#define INA226_BIT_POL              (0x0800)
#define INA226_BIT_CNVR             (0x0400)
#define INA226_BIT_AFF              (0x0010)
#define INA226_BIT_CVRF             (0x0008)
#define INA226_BIT_OVF              (0x0004)
#define INA226_BIT_APOL             (0x0002)
#define INA226_BIT_LEN              (0x0001)

typedef enum
{
    INA226_AVERAGES_1             = 0b000,
    INA226_AVERAGES_4             = 0b001,
    INA226_AVERAGES_16            = 0b010,
    INA226_AVERAGES_64            = 0b011,
    INA226_AVERAGES_128           = 0b100,
    INA226_AVERAGES_256           = 0b101,
    INA226_AVERAGES_512           = 0b110,
    INA226_AVERAGES_1024          = 0b111
} ina226_averages_t;

typedef enum
{
    INA226_BUS_CONV_TIME_140US    = 0b000,
    INA226_BUS_CONV_TIME_204US    = 0b001,
    INA226_BUS_CONV_TIME_332US    = 0b010,
    INA226_BUS_CONV_TIME_588US    = 0b011,
    INA226_BUS_CONV_TIME_1100US   = 0b100,
    INA226_BUS_CONV_TIME_2116US   = 0b101,
    INA226_BUS_CONV_TIME_4156US   = 0b110,
    INA226_BUS_CONV_TIME_8244US   = 0b111
} ina226_busConvTime_t;

typedef enum
{
    INA226_SHUNT_CONV_TIME_140US   = 0b000,
    INA226_SHUNT_CONV_TIME_204US   = 0b001,
    INA226_SHUNT_CONV_TIME_332US   = 0b010,
    INA226_SHUNT_CONV_TIME_588US   = 0b011,
    INA226_SHUNT_CONV_TIME_1100US  = 0b100,
    INA226_SHUNT_CONV_TIME_2116US  = 0b101,
    INA226_SHUNT_CONV_TIME_4156US  = 0b110,
    INA226_SHUNT_CONV_TIME_8244US  = 0b111
} ina226_shuntConvTime_t;

typedef enum
{
    INA226_MODE_POWER_DOWN      = 0b000,
    INA226_MODE_SHUNT_TRIG      = 0b001,
    INA226_MODE_BUS_TRIG        = 0b010,
    INA226_MODE_SHUNT_BUS_TRIG  = 0b011,
    INA226_MODE_ADC_OFF         = 0b100,
    INA226_MODE_SHUNT_CONT      = 0b101,
    INA226_MODE_BUS_CONT        = 0b110,
    INA226_MODE_SHUNT_BUS_CONT  = 0b111,
} ina226_mode_t;

bool INA226_configure(ina226_averages_t avg, ina226_busConvTime_t busConvTime, ina226_shuntConvTime_t shuntConvTime, ina226_mode_t mode);
bool INA226_calibrate(float rShuntValue, float iMaxExpected);
void INA226_checkConfig(void);

ina226_averages_t INA226_getAverages(void);
ina226_busConvTime_t INA226_getBusConversionTime(void);
ina226_shuntConvTime_t INA226_getShuntConversionTime(void);
ina226_mode_t INA226_getMode(void);

void INA226_enableShuntOverLimitAlert(void);
void INA226_enableShuntUnderLimitAlert(void);
void INA226_enableBusOvertLimitAlert(void);
void INA226_enableBusUnderLimitAlert(void);
void INA226_enableOverPowerLimitAlert(void);
void INA226_enableConversionReadyAlert(void);

void INA226_setBusVoltageLimit(float voltage);
void INA226_setShuntVoltageLimit(float voltage);
void INA226_setPowerLimit(float watts);

void INA226_setAlertInvertedPolarity(bool inverted);
void INA226_setAlertLatch(bool latch);

bool INA226_isMathOverflow(void);
bool INA226_isAlert(void);

float INA226_readShuntCurrent(void);
float INA226_readShuntVoltage(void);
float INA226_readBusPower(void);
float INA226_readBusVoltage(void);

float INA226_getMaxPossibleCurrent(void);
float INA226_getMaxCurrent(void);
float INA226_getMaxShuntVoltage(void);
float INA226_getMaxPower(void);

#endif //INA226_INA226_H
