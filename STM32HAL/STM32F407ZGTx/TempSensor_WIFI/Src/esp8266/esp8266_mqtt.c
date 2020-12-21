#include "esp8266_mqtt.h"
#include "esp8266_at.h"
#include "string.h"

//���ӳɹ���������Ӧ 20 02 00 00
//�ͻ��������Ͽ����� e0 00
const uint8_t parket_connetAck[] = {0x20,0x02,0x00,0x00};
const uint8_t parket_disconnet[] = {0xe0,0x00};
const uint8_t parket_heart[] = {0xc0,0x00};
const uint8_t parket_heart_reply[] = {0xc0,0x00};
const uint8_t parket_subAck[] = {0x90,0x03};

volatile uint16_t MQTT_TxLen;

//MQTT��������
void MQTT_SendBuf(uint8_t *buf, uint16_t len)
{
	ESP8266_ATSendBuf(buf, len);
}

//����������
void MQTT_SentHeart(void)
{
	MQTT_SendBuf((uint8_t *)parket_heart,sizeof(parket_heart));
}

//MQTT�������Ͽ�
void MQTT_Disconnect(void)
{
	MQTT_SendBuf((uint8_t *)parket_disconnet,sizeof(parket_disconnet));
}

//MQTT���ӷ������Ĵ������
uint8_t MQTT_Connect(char *ClientID, char *Username, char *Password)
{
	int ClientIDLen = strlen(ClientID);
	int UsernameLen = strlen(Username);
	int PasswordLen = strlen(Password);
	int DataLen;
  
	MQTT_TxLen = 0;
  memset(tx_buf, 0, sizeof(tx_buf));//��շ��ͻ���
  
	//�ɱ䱨ͷ+Payload  ÿ���ֶΰ��������ֽڵĳ��ȱ�ʶ
  DataLen = 10 + (ClientIDLen+2) + (UsernameLen+2) + (PasswordLen+2);
	
	//�̶���ͷ
	//���Ʊ�������
  tx_buf[MQTT_TxLen++] = 0x10;		//MQTT Message Type CONNECT
  
	//ʣ�೤��(�������̶�ͷ��)
	do{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if (DataLen > 0)
			encodedByte = encodedByte | 128;
		tx_buf[MQTT_TxLen++] = encodedByte;
	}while (DataLen > 0);
  
	//�ɱ䱨ͷ
	//Э����
	tx_buf[MQTT_TxLen++] = 0;        		// Protocol Name Length MSB    
	tx_buf[MQTT_TxLen++] = 4;        		// Protocol Name Length LSB    
	tx_buf[MQTT_TxLen++] = 'M';        	// ASCII Code for M    
	tx_buf[MQTT_TxLen++] = 'Q';        	// ASCII Code for Q    
	tx_buf[MQTT_TxLen++] = 'T';        	// ASCII Code for T    
	tx_buf[MQTT_TxLen++] = 'T';        	// ASCII Code for T    
	//Э�鼶��
	tx_buf[MQTT_TxLen++] = 4;        		// MQTT Protocol version = 4    
	//���ӱ�־
	tx_buf[MQTT_TxLen++] = 0xc2;        // conn flags 
	tx_buf[MQTT_TxLen++] = 0;        		// Keep-alive Time Length MSB    
	tx_buf[MQTT_TxLen++] = 60;        	// Keep-alive Time Length LSB  60S������  

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
		wait=30;//�ȴ�3sʱ��
		while(wait--)
		{
			//CONNECT
			if(rx_buf[0]==parket_connetAck[0] && rx_buf[1]==parket_connetAck[1]) //���ӳɹ�			   
			{
				return 1;//���ӳɹ�
			}
			HAL_Delay(100);			
		}
	}
  
  return 0;
  
}

//MQTT����/ȡ���������ݴ������
//topic       ���� 
//qos         ��Ϣ�ȼ� 
//whether     ����/ȡ�����������
uint8_t MQTT_SubscribeTopic(char *topic, uint8_t qos, uint8_t whether)
{    
	MQTT_TxLen=0;
  memset(tx_buf, 0, sizeof(tx_buf));//��շ��ͻ���
  
	int topiclen = strlen(topic);
	
	int DataLen = 2 + (topiclen+2) + (whether?1:0);//�ɱ䱨ͷ�ĳ��ȣ�2�ֽڣ�������Ч�غɵĳ���
	//�̶���ͷ
	//���Ʊ�������
	if(whether) tx_buf[MQTT_TxLen++] = 0x82; //��Ϣ���ͺͱ�־����
	else	tx_buf[MQTT_TxLen++] = 0xA2;    //ȡ������

	//ʣ�೤��
	do{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		tx_buf[MQTT_TxLen++] = encodedByte;
	}while ( DataLen > 0 );	
	
	//�ɱ䱨ͷ
	tx_buf[MQTT_TxLen++] = 0;				//��Ϣ��ʶ�� MSB
	tx_buf[MQTT_TxLen++] = 0x01;           //��Ϣ��ʶ�� LSB
	//��Ч�غ�
	tx_buf[MQTT_TxLen++] = BYTE1(topiclen);//���ⳤ�� MSB
	tx_buf[MQTT_TxLen++] = BYTE0(topiclen);//���ⳤ�� LSB   
	memcpy(&tx_buf[MQTT_TxLen], topic, topiclen);
	MQTT_TxLen += topiclen;

	if(whether)
	{
		tx_buf[MQTT_TxLen++] = qos;//QoS����
	}
	
	uint8_t cnt=2;
	uint8_t wait;
	while(cnt--)
	{
		memset(rx_buf,0,sizeof(rx_buf));
		MQTT_SendBuf(tx_buf, MQTT_TxLen);
		wait=30;//�ȴ�3sʱ��
		while(wait--)
		{
			if(rx_buf[0]==parket_subAck[0] && rx_buf[1]==parket_subAck[1]) //���ĳɹ�			   
			{
				return 1;//���ĳɹ�
			}
			HAL_Delay(100);			
		}
	}
	if(cnt) return 1;	//���ĳɹ�
	return 0;
}

//MQTT�������ݴ������
//topic   ���� 
//message ��Ϣ
//qos     ��Ϣ�ȼ� 
uint8_t MQTT_PublishData(char *topic, char *message, uint8_t qos)
{  
	int topicLength = strlen(topic);    
	int messageLength = strlen(message);     
	static uint16_t id=0;
	int DataLen;
  
	MQTT_TxLen=0;
  memset(tx_buf, 0, sizeof(tx_buf));//��շ��ͻ���
  
	//��Ч�غɵĳ����������㣺�ù̶���ͷ�е�ʣ�೤���ֶε�ֵ��ȥ�ɱ䱨ͷ�ĳ���
	//QOSΪ0ʱû�б�ʶ��
	//���ݳ���             ������   ���ı�ʶ��   ��Ч�غ�
	if(qos)	DataLen = (2+topicLength) + 2 + messageLength;       
	else	DataLen = (2+topicLength) + messageLength;   

  //�̶���ͷ
	//���Ʊ�������
	tx_buf[MQTT_TxLen++] = 0x30;    // MQTT Message Type PUBLISH  

	//ʣ�೤��
	do{
		uint8_t encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		tx_buf[MQTT_TxLen++] = encodedByte;
	}while ( DataLen > 0 );	
	
	tx_buf[MQTT_TxLen++] = BYTE1(topicLength);//���ⳤ��MSB
	tx_buf[MQTT_TxLen++] = BYTE0(topicLength);//���ⳤ��LSB 
	memcpy(&tx_buf[MQTT_TxLen], topic, topicLength);//��������
	MQTT_TxLen += topicLength;
        
	//���ı�ʶ��
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

