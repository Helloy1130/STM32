#ifndef _ESP8266_AT_H
#define _ESP8266_AT_H

#include "main.h"

#define RX_LEN        512
#define TX_LEN        256

extern uint8_t rx_byte;
extern uint8_t rx_index;
extern uint8_t rx_buf[RX_LEN];
extern uint8_t tx_buf[TX_LEN];

uint8_t ESP8266_Init(void);
void ESP8266_Restore(void);

void clear_rx_buf(void);
void ESP8266_ATSendString(char *str);//向ESP8266发送字符串
void ESP8266_ATSendBuf(uint8_t* buf,uint16_t len);//向ESP8266发送定长数据

uint8_t ESP8266_ConnectAP(char *ssid,char *pswd);		//ESP8266连接热点
uint8_t ESP8266_ConnectServer(char *mode, char *ip, uint16_t port);//使用指定协议(TCP/UDP)连接到服务器


#endif
