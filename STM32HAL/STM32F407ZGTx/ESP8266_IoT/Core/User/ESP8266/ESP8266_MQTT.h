#ifndef _ESP8266_MQTT_H
#define _ESP8266_MQTT_H

#include "main.h"

#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

uint8_t MQTT_Connect(char *ClientID, char *Username, char *Password);//MQTT���ӷ������Ĵ������
uint8_t MQTT_PublishData(char *topic, char *message, uint8_t qos);//MQTT�������ݴ������
uint8_t MQTT_SubscribeTopic(char *topic,uint8_t qos,uint8_t whether);//MQTT����/ȡ���������ݴ������
void MQTT_SentHeart(void);//����������
void MQTT_Disconnect(void);

#endif
