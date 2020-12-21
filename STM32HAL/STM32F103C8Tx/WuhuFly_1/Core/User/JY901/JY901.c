#include <string.h>
#include <stdio.h>
#include "JY901.h"
#include "structconfig.h"

FLOAT_ANGLE Att_Angle;//飞机姿态角
FLOAT_XYZ 	Gyr_rad;//角速度

bool JY901_recv_flag = false;

const uint8_t JY901_save_conf_cmd[5] = {0xFF,0xAA,0x00,0x00,0x00};
const uint8_t JY901_imu_cali_cmd[5]  = {0xFF,0xAA,0x01,0x01,0x00};
const uint8_t JY901_mag_cali_cmd[5]  = {0xFF,0xAA,0x01,0x02,0x01};
const uint8_t JY901_quit_cali_cmd[5] = {0xFF,0xAA,0x01,0x00,0x00};

uint8_t address_ = 0x50;  //默认设备地址0x50
bool transferMode_ = 0;
unsigned long lastTime;
uint8_t rxBuffer[12] = {0};
uint8_t rxCnt = 0;

bool dataTrue = true;

struct {
    struct {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint16_t milisecond;
    } time;
    struct {
        int16_t x;
        int16_t y;
        int16_t z;
        int16_t temperature;
    } acc;
    struct {
        int16_t x;
        int16_t y;
        int16_t z;
        int16_t temperature;
    } gyro;
    struct {
        int16_t roll;
        int16_t pitch;
        int16_t yaw;
        int16_t temperature;
    } angle;
    struct {
        int16_t x;
        int16_t y;
        int16_t z;
        int16_t temperature;
    } mag;
    struct {
        int16_t d_0;
        int16_t d_1;
        int16_t d_2;
        int16_t d_3;
    } dStatus;
    int32_t pressure;
    int32_t altitude;
    int32_t lon;
    int32_t lat;
    int16_t GPSHeight;
    int16_t GPSYaw;
    int32_t GPSVelocity;
} JY901_data;

void JY901_startIIC(uint8_t address)
{
    address_ = address;
    transferMode_ = 1;
}

bool JY901_readSerialData(uint8_t data)
{

    rxBuffer[rxCnt] = data;
    rxCnt++;
    if (rxBuffer[0] != 0x55) { //0x55为数据头识别码
        rxCnt = 0;
        return false;
    }

    if (rxCnt<11) {
        return false;
    }

    rxCnt = 0;  //归零计数
    uint8_t sum = 0;
    for (uint8_t cnt = 0; cnt<10; cnt++) {
        sum += rxBuffer[cnt];
    }
    if (sum != rxBuffer[10]) {
        return false;
    }

    switch (rxBuffer[1])
    {
        case 0x50:  memcpy(&JY901_data.time,    &rxBuffer[2], 8); break;    // 时间
        case 0x51:  memcpy(&JY901_data.acc,     &rxBuffer[2], 8); break;    //加速度
        case 0x52:  memcpy(&JY901_data.gyro,    &rxBuffer[2], 8); break;    //角速度
        case 0x53:  memcpy(&JY901_data.angle,   &rxBuffer[2], 8); break;    //角度
        case 0x54:  memcpy(&JY901_data.mag,     &rxBuffer[2], 8); break;    //磁场
        case 0x55:  memcpy(&JY901_data.dStatus, &rxBuffer[2], 8); break;    //端口状态
        case 0x56:  memcpy(&JY901_data.pressure,&rxBuffer[2], 4);           //气压
                    memcpy(&JY901_data.altitude,&rxBuffer[6], 4);           //高度
                    break;
        case 0x57:  memcpy(&JY901_data.lon,     &rxBuffer[2], 4);           //经度
                    memcpy(&JY901_data.lat,     &rxBuffer[6], 4);           //纬度
                    break;
        case 0x58:  memcpy(&JY901_data.GPSHeight,   &rxBuffer[2], 2);       //地速
                    memcpy(&JY901_data.GPSYaw,      &rxBuffer[4], 2);
                    memcpy(&JY901_data.GPSVelocity, &rxBuffer[6], 4);
                    break;
    }
    return true;
}

static void JY901_readRegisters(uint8_t deviceAddr, uint8_t addressToRead, uint8_t bytesToRead, uint8_t * dest)
{
//    Wire.beginTransmission(deviceAddr);
//    Wire.write(addressToRead);
//    Wire.endTransmission(false); //endTransmission but keep the connection active
//
//    Wire.requestFrom(deviceAddr, bytesToRead); //Ask for bytes, once done, bus is released by default
//
//    if (Wire.available() >= bytesToRead) {//Hang out until we get the # of bytes we expect
//        for (int x = 0; x < bytesToRead; x++)
//            dest[x] = Wire.read();
//        lastTime = millis();
//    }
}

static void JY901_writeRegister(uint8_t deviceAddr, uint8_t addressToWrite, uint8_t bytesToRead, uint8_t *dataToWrite)
{
//    Wire.beginTransmission(deviceAddr);
//    Wire.write(addressToWrite);
//    for (int i = 0; i < bytesToRead; i++)
//        Wire.write(dataToWrite[i]);
//    Wire.endTransmission(); //Stop transmitting
}

void JY901_readData(uint8_t address, uint8_t length, uint8_t data[])
{
    JY901_readRegisters(address_, address, length, data);
}

uint16_t JY901_getTime(const char* str)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_YYMM, 8, (uint8_t*)&JY901_data.time);

    if (strcmp(str, "year") == 0)       //年
        return JY901_data.time.year;

    if (strcmp(str, "month") == 0)      //月
        return JY901_data.time.month;

    if (strcmp(str, "day") == 0)        //日
        return JY901_data.time.day;

    if (strcmp(str, "hour") == 0)       //时
        return JY901_data.time.hour;

    if (strcmp(str, "minute") == 0)     //分
        return JY901_data.time.minute;

    if (strcmp(str, "second") == 0)     //秒
        return JY901_data.time.second;

    if (strcmp(str, "milisecond") == 0) //毫秒
        return JY901_data.time.milisecond;

    return 0;
}

double JY901_getAccX(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_AX, 2, (uint8_t *)&JY901_data.acc.x);
    return JY901_data.acc.x / 2048.0;
//    return JY901_data.acc.x;
}


double JY901_getAccY(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_AY, 2, (uint8_t *)&JY901_data.acc.y);
    return JY901_data.acc.y / 2048.0;
}

double JY901_getAccZ(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_AZ, 2, (uint8_t *)&JY901_data.acc.z);
    return JY901_data.acc.z / 2048.0;
}

double JY901_getGyroX(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_GX, 2, (uint8_t *)&JY901_data.gyro.x);
    return JY901_data.gyro.x / (32768.0/2000.0);
}

double JY901_getGyroY(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_GY, 2, (uint8_t *)&JY901_data.gyro.y);
    return JY901_data.gyro.y / (32768.0/2000.0);
}

double JY901_getGyroZ(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_GZ, 2, (uint8_t *)&JY901_data.gyro.z);
    return JY901_data.gyro.z / (32768.0/2000.0);
}

double JY901_getMagX(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_HX, 2, (uint8_t *)&JY901_data.mag.x);
    return JY901_data.mag.x / (32768.0/180.0);
}

double JY901_getMagY(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_HY, 2, (uint8_t *)&JY901_data.mag.y);
    return JY901_data.mag.y / (32768.0/180.0);
}

double JY901_getMagZ(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_HZ, 2, (uint8_t *)&JY901_data.mag.z);
    return JY901_data.mag.z / (32768.0/180.0);
}

int16_t JY901_getAccRawX(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_AX, 2, (uint8_t *)&JY901_data.acc.x);
    return JY901_data.acc.x;
}

int16_t JY901_getAccRawY(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_AY, 2, (uint8_t *)&JY901_data.acc.y);
    return JY901_data.acc.y;
}

int16_t JY901_getAccRawZ(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_AZ, 2, (uint8_t *)&JY901_data.acc.z);
    return JY901_data.acc.z;
}

int16_t JY901_getGyroRawX(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_GX, 2, (uint8_t *)&JY901_data.gyro.x);
    return JY901_data.gyro.x;
}

int16_t JY901_getGyroRawY(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_GY, 2, (uint8_t *)&JY901_data.gyro.y);
    return JY901_data.gyro.y;
}

int16_t JY901_getGyroRawZ(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_GZ, 2, (uint8_t *)&JY901_data.gyro.z);
    return JY901_data.gyro.z;
}

int16_t JY901_getMagRawX(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_HX, 2, (uint8_t *)&JY901_data.mag.x);
    return JY901_data.mag.x;
}

int16_t JY901_getMagRawY(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_HY, 2, (uint8_t *)&JY901_data.mag.y);
    return JY901_data.mag.y;
}

int16_t JY901_getMagRawZ(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_HZ, 2, (uint8_t *)&JY901_data.mag.z);
    return JY901_data.mag.z;
}

double JY901_getRoll(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_Roll, 2, (uint8_t *)&JY901_data.angle.roll);
    return JY901_data.angle.roll / (32768.0/180.0);
}

double JY901_getPitch(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_Pitch, 2, (uint8_t *)&JY901_data.angle.pitch);
    return JY901_data.angle.pitch / (32768.0/180.0);
}

double JY901_getYaw(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_Yaw, 2, (uint8_t *)&JY901_data.angle.yaw);
    return JY901_data.angle.yaw / (32768.0/180.0);
}


// double JY901_getTemp(void)
// {
// 	if (transferMode_)
// 		JY901_readRegisters(address_, JY_HX, 8, (uint8_t *)&JY901_data.mag);
// 	if (strcmp(str, "t") == 0 || strcmp(str, "T") == 0) {    //温度
// 		return JY901_data.acc.temperature / 340.0 + 36.53;
// 	}
// }

int32_t JY901_getPressure(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_PressureL, 4, (uint8_t *)&JY901_data.pressure);

    return JY901_data.pressure; //Pa
}

int32_t JY901_getAltitude(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_HeightL, 4, (uint8_t *)&JY901_data.altitude);

    return JY901_data.altitude; //cm
}

int16_t JY901_getD0Status(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_D0Status, 2, (uint8_t *)&JY901_data.dStatus.d_0);
    return JY901_data.dStatus.d_0;
}

int16_t JY901_getD1Status(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_D1Status, 2, (uint8_t *)&JY901_data.dStatus.d_1);
    return JY901_data.dStatus.d_1;
}

int16_t JY901_getD2Status(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_D2Status, 2, (uint8_t *)&JY901_data.dStatus.d_2);
    return JY901_data.dStatus.d_2;
}

int16_t JY901_getD3Status(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_D3Status, 2, (uint8_t *)&JY901_data.dStatus.d_3);
    return JY901_data.dStatus.d_3;
}

int32_t JY901_getLon(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_LonL, 4, (uint8_t *)&JY901_data.lon);

    return JY901_data.lon;
}

int32_t JY901_getLat(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_LatL, 4, (uint8_t *)&JY901_data.lat);

    return JY901_data.lat;
}

double JY901_getGPSH(void)
{
    if (transferMode_)
        JY901_readRegisters(address_, JY_GPSHeight, 2, (uint8_t *)&JY901_data.GPSHeight);

    return JY901_data.GPSHeight / 10.0;
}

double JY901_getGPSY(void){
    if (transferMode_)
        JY901_readRegisters(address_, JY_GPSYAW, 2, (uint8_t *)&JY901_data.GPSYaw);

    return JY901_data.GPSYaw / 10.0;
}

double JY901_getGPSV(void){
    if (transferMode_)
        JY901_readRegisters(address_, JY_GPSVL, 4, (uint8_t *)&JY901_data.GPSVelocity);

    return JY901_data.GPSVelocity / 1000.0;
}

void JY901_saveConf(void)
{
    if (transferMode_) {
        uint8_t cmd[2] = {0x00,0x00};
        JY901_writeRegister(address_, JY_SAVE, 2, cmd);
    }
}

void JY901_quitCali(void)
{
    if (transferMode_) {
        uint8_t cmd[2] = {0x00,0x00};
        JY901_writeRegister(address_, JY_CALSW, 2, cmd);
    }
}

void JY901_caliIMU(void)
{
    if (transferMode_) {
        uint8_t cmd[2] = {0x01,0x00};
        JY901_writeRegister(address_, JY_CALSW, 2, cmd);
    }
}

void JY901_caliMag(void)
{
    if (transferMode_) {
        uint8_t cmd[2] = {0x02,0x00};
        JY901_writeRegister(address_, JY_CALSW, 2, cmd);
    }
}

unsigned long JY901_getLastTime(void)
{
    return lastTime;
}
