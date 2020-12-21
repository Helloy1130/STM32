#ifndef _ESP8266_MQTT_H
#define _ESP8266_MQTT_H

#include "main.h"

#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

uint8_t MQTT_Connect(char *ClientID, char *Username, char *Password);//MQTT连接服务器的打包函数
uint8_t MQTT_PublishData(char *topic, char *message, uint8_t qos);//MQTT发布数据打包函数
uint8_t MQTT_SubscribeTopic(char *topic,uint8_t qos,uint8_t whether);//MQTT订阅/取消订阅数据打包函数
void MQTT_SentHeart(void);//发送心跳包
void MQTT_Disconnect(void);

#endif
