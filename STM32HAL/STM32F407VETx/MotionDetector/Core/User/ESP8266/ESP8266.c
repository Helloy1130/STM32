//
// Created by Y on 2020/12/17.
//

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ESP8266.h"
#include "usart.h"

uint8_t rx_byte;
uint32_t rx_index = 0;
uint8_t rx_buf[RX_LEN];
uint8_t tx_buf[TX_LEN];

void Clear_rx_buf(void)
{
    rx_index = 0;
    memset(rx_buf, 0x00, RX_LEN);
}

//��ESP8266���Ͷ�������
void ESP8266_ATSendBuf(uint8_t* buf,uint16_t len)
{
    Clear_rx_buf();
    //��������
    HAL_UART_Transmit(&huart6, (uint8_t *)buf, len, 0xFFFF);
}

//��ESP8266�����ַ���
void ESP8266_ATSendString(char *str)
{
    Clear_rx_buf();
    HAL_UART_Transmit(&huart6, (uint8_t *)str, strlen(str), 0xFFFF);
}

//�����ַ������Ƿ������һ���ַ���
uint8_t FindStr(char *dest, char *src, uint16_t retry_nms)
{
    retry_nms /= 10;                           //��ʱʱ��

    while(strstr(dest,src)==0 && retry_nms)//�ȴ����ڽ�����ϻ�ʱ�˳�
    {
        HAL_Delay(10);
        retry_nms--;
    }

    if(retry_nms) {
        return 1;  //����Ӧʱ����,�򷵻�1
    }

    return 0;
}

//�˳�͸��
void ESP8266_ExitUnvarnishedTrans(void)
{
    ESP8266_ATSendString("+++");
    HAL_Delay(50);
    ESP8266_ATSendString("+++");
    HAL_Delay(50);
}

//����͸��ģʽ
static uint8_t ESP8266_OpenTransmission(void)
{
    uint8_t cnt=2;
    while(cnt--)
    {
        ESP8266_ATSendString("AT+CIPMODE=1\r\n");
        if(FindStr((char*)rx_buf,"OK",200)!=0)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * ���ܣ����ESP8266�Ƿ�����
 * ������None
 * ����ֵ��ESP8266����״̬
 *        ��0 ESP8266����
 *        0 ESP8266������
 */
static uint8_t ESP8266_Check(void)
{
    uint8_t check_cnt = 5;
    while(check_cnt--)
    {
        ESP8266_ATSendString("AT\r\n");         //����AT����ָ��

        if(FindStr((char*)rx_buf,"OK",200) != 0)//����OK
        {
            return 1;
        }
    }

    return 0;
}

/**
 * ���ܣ���ʼ��ESP8266
 * ������None
 * ����ֵ����ʼ���������0Ϊ��ʼ���ɹ�,0Ϊʧ��
 */
uint8_t ESP8266_Init(void)
{
    ESP8266_ExitUnvarnishedTrans();		//�˳�͸��
    HAL_Delay(500);
    ESP8266_ATSendString("AT+RST\r\n");
    HAL_Delay(800);
    if(!ESP8266_Check()){             //ʹ��ATָ����ESP8266�Ƿ����
        return 0;
    }

    ESP8266_ATSendString("ATE0\r\n");     	//�رջ���

    if(FindStr((char*)rx_buf,"OK",500) == 0)  //���ò��ɹ�
    {
        return 0;
    }

    return 1;
}

/**
 * ���ܣ��ָ���������
 * ������None
 * ����ֵ��None
 * ˵��:��ʱESP8266�е��û����ý�ȫ����ʧ�ظ��ɳ���״̬
 */
void ESP8266_Restore(void)
{
    ESP8266_ExitUnvarnishedTrans();          	//�˳�͸��
    HAL_Delay(500);
    ESP8266_ATSendString("AT+RESTORE\r\n");		//�ָ�����
}

/**
 * ���ܣ������ȵ�
 * ������
 *         ssid:�ȵ���
 *         pswd:�ȵ�����
 * ����ֵ��
 *         ���ӽ��,��0���ӳɹ�,0����ʧ��
 * ˵����
 *         ʧ�ܵ�ԭ�������¼���(UARTͨ�ź�ESP8266���������)
 *         1. WIFI�������벻��ȷ
 *         2. ·���������豸̫��,δ�ܸ�ESP8266����IP
 */
uint8_t ESP8266_ConnectAP(char *ssid,char *pswd)
{
    uint8_t cnt = 5;
    while(cnt--)
    {
        ESP8266_ATSendString("AT+CWMODE_CUR=1\r\n");              //����ΪSTATIONģʽ
        if(FindStr((char*)rx_buf,"OK",200) != 0)
        {
            break;
        }
    }
    if(cnt == 0){
        return 0;
    }

    cnt = 2;
    while(cnt--)
    {
        memset(tx_buf, 0, sizeof(tx_buf));//��շ��ͻ���
        sprintf((char*)tx_buf, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", ssid, pswd);//����Ŀ��AP
        ESP8266_ATSendString((char*)tx_buf);
        if(FindStr((char*)rx_buf,"OK",8000) != 0)    //���ӳɹ��ҷ��䵽IP
        {
            return 1;
        }
    }
    return 0;
}

/**
 * ���ܣ�ʹ��ָ��Э��(TCP/UDP)���ӵ�������
 * ������
 *         mode:Э������ "TCP","UDP"
 *         ip:Ŀ�������IP
 *         port:Ŀ���Ƿ������˿ں�
 * ����ֵ��
 *         ���ӽ��,��0���ӳɹ�,0����ʧ��
 * ˵����
 *         ʧ�ܵ�ԭ�������¼���(UARTͨ�ź�ESP8266���������)
 *         1. Զ�̷�����IP�Ͷ˿ں�����
 *         2. δ����AP
 *         3. �������˽�ֹ���(һ�㲻�ᷢ��)
 */
uint8_t ESP8266_ConnectServer(char *mode, char *ip, uint16_t port)
{
    uint8_t cnt;

    ESP8266_ExitUnvarnishedTrans();                   //����������˳�͸��
    HAL_Delay(500);

    //���ӷ�����
    cnt = 2;
    while(cnt--)
    {
        memset(tx_buf, 0, sizeof(tx_buf));//��շ��ͻ���
        sprintf((char*)tx_buf, "AT+CIPSTART=\"%s\",\"%s\",%d\r\n", mode, ip, port);
        ESP8266_ATSendString((char*)tx_buf);
        if(FindStr((char*)rx_buf,"OK",8000) != 0)
        {
            break;
        }
    }
    if(cnt == 0) {
        return 0;
    }

    //����͸��ģʽ
    if(ESP8266_OpenTransmission() == 0) return 0;

    //��������״̬
    cnt = 2;
    while(cnt--)
    {
        ESP8266_ATSendString("AT+CIPSEND\r\n");//��ʼ����͸������״̬
        if(FindStr((char*)rx_buf,">",200) != 0)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * ���ܣ������ͷ������Ͽ�����
 * ������None
 * ����ֵ��
 *         ���ӽ��,��0�Ͽ��ɹ�,0�Ͽ�ʧ��
 */
uint8_t DisconnectServer(void)
{
    uint8_t cnt = 5;

    ESP8266_ExitUnvarnishedTrans();	//�˳�͸��
    HAL_Delay(500);

    while(cnt--)
    {
        ESP8266_ATSendString("AT+CIPCLOSE\r\n");//�ر�����

        if(FindStr((char*)rx_buf,"CLOSED",200) != 0)//�����ɹ�,�ͷ������ɹ��Ͽ�
        {
            break;
        }
    }
    if(cnt) return 1;

    return 0;
}

/**
 * ���ܣ�APģʽ,UDP����
 * ������
 *         ssid:WIFI��
 *         pswd:WIFI����
 * ����ֵ��
 *         ���ӽ��,��0���ӳɹ�,0����ʧ��
 */
uint8_t ESP8266_AP(char *ssid,char *pswd)
{
    uint8_t cnt = 2;
    while(cnt--)
    {
        ESP8266_ATSendString("AT+CWMODE=2\r\n");              //����ΪAPģʽ
        if(FindStr((char*)rx_buf,"OK",200) != 0)
        {
            break;
        }
    }
    if(cnt == 0){
        return 0;
    }
    HAL_Delay(100);

    ESP8266_ATSendString("AT+RST\r\n");
    HAL_Delay(800);

    cnt = 2;
    while(cnt--)
    {
        memset(tx_buf, 0, sizeof(tx_buf));//��շ��ͻ���
        sprintf((char*)tx_buf, "AT+CWSAP=\"%s\",\"%s\",1,%d\r\n", ssid, pswd, 3);//����AP
        ESP8266_ATSendString((char*)tx_buf);
        if(FindStr((char*)rx_buf,"OK",8000) != 0)    //�����ɹ�
        {
            break;
        }
    }
    if(cnt == 0){
        return 0;
    }
    HAL_Delay(100);

    cnt = 2;
    while(cnt--)
    {
        ESP8266_ATSendString("AT+CIPMUX=0\r\n");
        if(FindStr((char*)rx_buf,"OK",8000) != 0)    //�����ɹ�
        {
            break;
        }
    }
    if(cnt == 0){
        return 0;
    }
    HAL_Delay(100);

    return 1;
}

uint8_t ESP8266_UDP_Connect(char *ip,int port)
{
    uint8_t cnt = 2;
    while(cnt--)
    {
        memset(tx_buf, 0, sizeof(tx_buf));//��շ��ͻ���
        sprintf((char*)tx_buf, "AT+CIPSTART=\"UDP\",\"%s\",%d\r\n", ip, port);//����UDP����
        ESP8266_ATSendString((char*)tx_buf);
        if(FindStr((char*)rx_buf,"OK",8000) != 0)    //�����ɹ�
        {
            break;
        }
    }
    if(cnt == 0){
        return 0;
    }

    return 1;
}

uint8_t ESP8266_UDP_Send(char *data)
{
    uint8_t cnt = 2;
    while(cnt--)
    {
        memset(tx_buf, 0, sizeof(tx_buf));//��շ��ͻ���
        sprintf((char*)tx_buf, "AT+CIPSEND=%d\r\n", sizeof(data));//���ݳ���
        ESP8266_ATSendString((char*)tx_buf);
        if(FindStr((char*)rx_buf,"OK",8000) != 0)
        {
            break;
        }
    }
    if(cnt == 0){
        return 0;
    }

    cnt = 2;
    while(cnt--)
    {
        ESP8266_ATSendString(data);
        if(FindStr((char*)rx_buf,"OK",8000) != 0)
        {
            break;
        }
    }
    if(cnt == 0){
        return 0;
    }

    return 1;
}