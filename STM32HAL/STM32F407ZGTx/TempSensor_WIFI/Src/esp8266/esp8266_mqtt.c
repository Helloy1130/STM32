#include "esp8266_mqtt.h"
#include "esp8266_at.h"
#include "string.h"

//连接成功服务器回应 20 02 00 00
//客户端主动断开连接 e0 00
const uint8_t parket_connetAck[] = {0x20,0x02,0x00,0x00};
const uint8_t parket_disconnet[] = {0xe0,0x00};
const uint8_t parket_heart[] = {0xc0,0x00};
const uint8_t parket_heart_reply[] = {0xc0,0x00};
const uint8_t parket_subAck[] = {0x90,0x03};

volatile uint16_t MQTT_TxLen;

//MQTT发送数据
void MQTT_SendBuf(uint8_t *buf, uint16_t len)
{
	ESP8266_ATSendBuf(buf, len);
}

//发送心跳包
void MQTT_SentHeart(void)
{
	MQTT_SendBuf((uint8_t *)parket_heart,sizeof(parket_heart));
}

//MQTT无条件断开
void MQTT_Disconnect(void)
{
	MQTT_SendBuf((uint8_t *)parket_disconnet,sizeof(parket_disconnet));
}

//MQTT连接服务器的打包函数
uint8_t MQTT_Connect(char *ClientID, char *Username, char *Password)
{
	int ClientIDLen = strlen(ClientID);
	int UsernameLen = strlen(Username);
	int PasswordLen = strlen(Password);
	int DataLen;
  
	MQTT_TxLen = 0;
  memset(tx_buf, 0, sizeof(tx_buf));//清空发送缓冲
  
	//可变报头+Payload  每个字段包含两个字节的长度标识
  DataLen = 10 + (ClientIDLen+2) + (UsernameLen+2) + (PasswordLen+2);
	
	//固定报头
	//控制报文类型
  tx_buf[MQTT_TxLen++] = 0x10;		//MQTT Message Type CONNECT
  
	//剩余长度(不包括固定头部)
	do{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if (DataLen > 0)
			encodedByte = encodedByte | 128;
		tx_buf[MQTT_TxLen++] = encodedByte;
	}while (DataLen > 0);
  
	//可变报头
	//协议名
	tx_buf[MQTT_TxLen++] = 0;        		// Protocol Name Length MSB    
	tx_buf[MQTT_TxLen++] = 4;        		// Protocol Name Length LSB    
	tx_buf[MQTT_TxLen++] = 'M';        	// ASCII Code for M    
	tx_buf[MQTT_TxLen++] = 'Q';        	// ASCII Code for Q    
	tx_buf[MQTT_TxLen++] = 'T';        	// ASCII Code for T    
	tx_buf[MQTT_TxLen++] = 'T';        	// ASCII Code for T    
	//协议级别
	tx_buf[MQTT_TxLen++] = 4;        		// MQTT Protocol version = 4    
	//连接标志
	tx_buf[MQTT_TxLen++] = 0xc2;        // conn flags 
	tx_buf[MQTT_TxLen++] = 0;        		// Keep-alive Time Length MSB    
	tx_buf[MQTT_TxLen++] = 60;        	// Keep-alive Time Length LSB  60S心跳包  

	tx_buf[MQTT_TxLen++] = BYTE1(ClientIDLen);// Client ID length MSB    
	tx_buf[MQTT_TxLen++] = BYTE0(ClientIDLen);// Client ID length LSB  	
	memcpy(&tx_buf[MQTT_TxLen], ClientID, ClientIDLen);
	MQTT_TxLen += ClientIDLen;
  
	if(UsernameLen > 0)
	{   
		tx_buf[MQTT_TxLen++] = BYTE1(UsernameLen);		//username length MSB    
		tx_buf[MQTT_TxLen++] = BYTE0(UsernameLen);    //username length LSB    
		memcpy(&tx_buf[MQTT_TxLen], Username, UsernameLen);
		MQTT_TxLen += UsernameLen;
	}
	
	if(PasswordLen > 0)
	{    
		tx_buf[MQTT_TxLen++] = BYTE1(PasswordLen);		//password length MSB    
		tx_buf[MQTT_TxLen++] = BYTE0(PasswordLen);    //password length LSB  
		memcpy(&tx_buf[MQTT_TxLen],Password,PasswordLen);
		MQTT_TxLen += PasswordLen; 
	}    
	
	uint8_t cnt=2;
	uint8_t wait;
	while(cnt--)
	{
		MQTT_SendBuf(tx_buf, MQTT_TxLen);
		wait=30;//等待3s时间
		while(wait--)
		{
			//CONNECT
			if(rx_buf[0]==parket_connetAck[0] && rx_buf[1]==parket_connetAck[1]) //连接成功			   
			{
				return 1;//连接成功
			}
			HAL_Delay(100);			
		}
	}
  
  return 0;
  
}

//MQTT订阅/取消订阅数据打包函数
//topic       主题 
//qos         消息等级 
//whether     订阅/取消订阅请求包
uint8_t MQTT_SubscribeTopic(char *topic, uint8_t qos, uint8_t whether)
{    
	MQTT_TxLen=0;
  memset(tx_buf, 0, sizeof(tx_buf));//清空发送缓冲
  
	int topiclen = strlen(topic);
	
	int DataLen = 2 + (topiclen+2) + (whether?1:0);//可变报头的长度（2字节）加上有效载荷的长度
	//固定报头
	//控制报文类型
	if(whether) tx_buf[MQTT_TxLen++] = 0x82; //消息类型和标志订阅
	else	tx_buf[MQTT_TxLen++] = 0xA2;    //取消订阅

	//剩余长度
	do{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		tx_buf[MQTT_TxLen++] = encodedByte;
	}while ( DataLen > 0 );	
	
	//可变报头
	tx_buf[MQTT_TxLen++] = 0;				//消息标识符 MSB
	tx_buf[MQTT_TxLen++] = 0x01;           //消息标识符 LSB
	//有效载荷
	tx_buf[MQTT_TxLen++] = BYTE1(topiclen);//主题长度 MSB
	tx_buf[MQTT_TxLen++] = BYTE0(topiclen);//主题长度 LSB   
	memcpy(&tx_buf[MQTT_TxLen], topic, topiclen);
	MQTT_TxLen += topiclen;

	if(whether)
	{
		tx_buf[MQTT_TxLen++] = qos;//QoS级别
	}
	
	uint8_t cnt=2;
	uint8_t wait;
	while(cnt--)
	{
		memset(rx_buf,0,sizeof(rx_buf));
		MQTT_SendBuf(tx_buf, MQTT_TxLen);
		wait=30;//等待3s时间
		while(wait--)
		{
			if(rx_buf[0]==parket_subAck[0] && rx_buf[1]==parket_subAck[1]) //订阅成功			   
			{
				return 1;//订阅成功
			}
			HAL_Delay(100);			
		}
	}
	if(cnt) return 1;	//订阅成功
	return 0;
}

//MQTT发布数据打包函数
//topic   主题 
//message 消息
//qos     消息等级 
uint8_t MQTT_PublishData(char *topic, char *message, uint8_t qos)
{  
	int topicLength = strlen(topic);    
	int messageLength = strlen(message);     
	static uint16_t id=0;
	int DataLen;
  
	MQTT_TxLen=0;
  memset(tx_buf, 0, sizeof(tx_buf));//清空发送缓冲
  
	//有效载荷的长度这样计算：用固定报头中的剩余长度字段的值减去可变报头的长度
	//QOS为0时没有标识符
	//数据长度             主题名   报文标识符   有效载荷
	if(qos)	DataLen = (2+topicLength) + 2 + messageLength;       
	else	DataLen = (2+topicLength) + messageLength;   

  //固定报头
	//控制报文类型
	tx_buf[MQTT_TxLen++] = 0x30;    // MQTT Message Type PUBLISH  

	//剩余长度
	do{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		tx_buf[MQTT_TxLen++] = encodedByte;
	}while ( DataLen > 0 );	
	
	tx_buf[MQTT_TxLen++] = BYTE1(topicLength);//主题长度MSB
	tx_buf[MQTT_TxLen++] = BYTE0(topicLength);//主题长度LSB 
	memcpy(&tx_buf[MQTT_TxLen], topic, topicLength);//拷贝主题
	MQTT_TxLen += topicLength;
        
	//报文标识符
	if(qos)
	{
			tx_buf[MQTT_TxLen++] = BYTE1(id);
			tx_buf[MQTT_TxLen++] = BYTE0(id);
			id++;
	}
	memcpy(&tx_buf[MQTT_TxLen],message,messageLength);
  MQTT_TxLen += messageLength;
        
	MQTT_SendBuf(tx_buf, MQTT_TxLen);
  
  return MQTT_TxLen;
}

