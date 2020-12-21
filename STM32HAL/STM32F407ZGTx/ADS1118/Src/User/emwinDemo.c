#include "DIALOG.h"
#include "emwinDemo.h"
#include "stdio.h"

/*******************************************
DDS_mode:
			1:��Ƶ
			2:AM;
			3:FM;
			4:PSK
			5:ASK
			6:����VPP
*******************************************/
uint8_t DDS_mode;

//Ƶ��
uint32_t Frequency;

//AM���ƶ�10%-100%   --->   	SPI���ݣ�1-10
uint32_t am = 10;

//FM���ƶ�Ƶƫ5kHz 	---> 	SPI���ݣ�1 	Ƶƫ10kHz ---> SPI���ݣ�2
uint32_t fm = 2;

//����VPP,10%-100%	--->   	SPI���ݣ�1-10
uint32_t vpp = 100;

char buf[10];//������ʾ
uint32_t num;//��ǰ�趨ֵ

WM_HWIN hWin_user;

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x03)
#define ID_BUTTON_2 (GUI_ID_USER + 0x04)
#define ID_BUTTON_3 (GUI_ID_USER + 0x05)
#define ID_BUTTON_4 (GUI_ID_USER + 0x06)
#define ID_BUTTON_5 (GUI_ID_USER + 0x07)
#define ID_BUTTON_6 (GUI_ID_USER + 0x08)
#define ID_BUTTON_7 (GUI_ID_USER + 0x09)
#define ID_BUTTON_8 (GUI_ID_USER + 0x0A)
#define ID_BUTTON_9 (GUI_ID_USER + 0x0C)
#define ID_BUTTON_10 (GUI_ID_USER + 0x0D)
#define ID_BUTTON_11 (GUI_ID_USER + 0x0E)
#define ID_BUTTON_12 (GUI_ID_USER + 0x0F)
#define ID_BUTTON_13 (GUI_ID_USER + 0x10)
#define ID_BUTTON_14 (GUI_ID_USER + 0x11)
#define ID_BUTTON_15 (GUI_ID_USER + 0x12)
#define ID_BUTTON_16 (GUI_ID_USER + 0x13)
#define ID_BUTTON_17 (GUI_ID_USER + 0x14)
#define ID_BUTTON_18 (GUI_ID_USER + 0x15)
#define ID_TEXT_0 (GUI_ID_USER + 0x16)
#define ID_TEXT_1 (GUI_ID_USER + 0x17)
#define ID_TEXT_2 (GUI_ID_USER + 0x18)
#define ID_TEXT_3 (GUI_ID_USER + 0x19)
#define ID_TEXT_4 (GUI_ID_USER + 0x1A)
#define ID_TEXT_5 (GUI_ID_USER + 0x1B)
#define ID_TEXT_6 (GUI_ID_USER + 0x1C)
#define ID_TEXT_7 (GUI_ID_USER + 0x1D)
#define ID_TEXT_8 (GUI_ID_USER + 0x1E)
#define ID_TEXT_9 (GUI_ID_USER + 0x1F)
#define ID_TEXT_10 (GUI_ID_USER + 0x20)
#define ID_TEXT_11 (GUI_ID_USER + 0x21)
#define ID_TEXT_12 (GUI_ID_USER + 0x22)

//�Ի�����Դ��
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
        { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 480, 800, 0, 0x64, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 10, 500, 120, 80, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 150, 500, 120, 80, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_2, 10, 590, 120, 80, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_3, 150, 590, 120, 80, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_4, 10, 680, 120, 80, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_5, 150, 680, 120, 80, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_6, 330, 680, 120, 80, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_7, 20, 190, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_8, 185, 190, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_9, 350, 190, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_10, 20, 260, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_11, 185, 260, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_12, 350, 260, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_13, 20, 330, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_14, 185, 330, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_15, 350, 330, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_16, 20, 400, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_17, 185, 400, 100, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_18, 350, 400, 100, 60, 0, 0x0, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_0, 10, 10, 80, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_1, 150, 10, 80, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_2, 300, 10, 90, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_3, 10, 50, 80, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_4, 300, 50, 85, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_5, 10, 90, 140, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_6, 90, 10, 55, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_7, 235, 10, 55, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_8, 395, 10, 55, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_9, 90, 50, 150, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_10, 390, 50, 50, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_11, 150, 90, 150, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_12, 50, 130, 200, 20, 0, 0x64, 0 },
};

//�Ի���ص�����
static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    WM_HWIN hWin = pMsg->hWin;

    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
            //��ʼ���Ի���
            hItem = pMsg->hWin;
            FRAMEWIN_SetText(hItem, "DDS");
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetFont(hItem, GUI_FONT_24_ASCII);
            //��ʼ��BUTTON
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
            BUTTON_SetText(hItem, "AM");

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
            BUTTON_SetText(hItem, "FM");

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
            BUTTON_SetText(hItem, "PSK");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
            BUTTON_SetText(hItem, "ASK");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
            BUTTON_SetText(hItem, "FRE");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
            BUTTON_SetText(hItem, "VPP");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
            BUTTON_SetText(hItem, "OK");

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_7);
            BUTTON_SetText(hItem, "1");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_8);
            BUTTON_SetText(hItem, "2");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_9);
            BUTTON_SetText(hItem, "3");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_10);
            BUTTON_SetText(hItem, "4");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_11);
            BUTTON_SetText(hItem, "5");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_12);
            BUTTON_SetText(hItem, "6");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_13);
            BUTTON_SetText(hItem, "7");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_14);
            BUTTON_SetText(hItem, "8");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_15);
            BUTTON_SetText(hItem, "9");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_16);
            BUTTON_SetText(hItem, "C");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_17);
            BUTTON_SetText(hItem, "0");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_18);
            BUTTON_SetText(hItem, "<--");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
            //��ʼ��TEXT
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetText(hItem, "MODE:");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
            TEXT_SetText(hItem, "AM(%):");
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
            TEXT_SetText(hItem, "FM(kHz):");
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
            TEXT_SetText(hItem, "SET(Hz):");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
            TEXT_SetText(hItem, "VPP(%):");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
            TEXT_SetText(hItem, "Frequency(Hz):");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_11);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_12);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            break;
        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            switch(Id)
            {
                case ID_BUTTON_0: //AM
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED://��ť�����²��ͷ�
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_6), "AM");
                            DDS_mode = 2;
                            break;
                    }
                    break;
                case ID_BUTTON_1: //FM
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_6), "FM");
                            DDS_mode = 3;
                            break;
                    }
                    break;
                case ID_BUTTON_2: //PSK
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_6), "PSK");
                            DDS_mode = 4;
                            break;
                    }
                    break;
                case ID_BUTTON_3: //ASK
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_6), "ASK");
                            DDS_mode = 5;
                            break;
                    }
                    break;
                case ID_BUTTON_4: // FRE
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_6), "FRE");
                            DDS_mode = 1;
                            break;
                    }
                    break;
                case ID_BUTTON_5: //VPP
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_6), "VPP");
                            DDS_mode = 6;
                            break;
                    }
                    break;
                case ID_BUTTON_6: //OK
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1) //��Ƶ
                            {
                                if(num<100000000)//100M��������ʧ��
                                {
                                    //��ʾ
                                    Frequency  = num;
                                    sprintf(buf, "%d", Frequency);//��ʾƵ��
                                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_11), buf);
                                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "Frequency Setted!");
                                    GUI_Delay(1000);
                                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "");
                                }
                                else
                                {
                                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "Out of range!");
                                    GUI_Delay(1000);
                                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "");
                                }
                            }
                            if(DDS_mode==2)//AM
                            {
                                //��ʾ
                                am = 10;//Ĭ�ϵ��ƶ�Ϊ10%
                                sprintf(buf, "%d", am);//��ʾ���ƶ�
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_7), buf);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "AM Setted!");
                                GUI_Delay(1000);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "");
                            }
                            if(DDS_mode==3)//FM
                            {
                                //��ʾ
                                fm = 2;//Ĭ�����ƵƫΪ10kHz
                                sprintf(buf, "%d", fm*5);//��ʾ���Ƶƫ
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_8), buf);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "FM Setted!");
                                GUI_Delay(1000);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "");
                            }
                            if(DDS_mode==4)//PSK
                            {
                                //��ʾ
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "PSK Setted!");
                                GUI_Delay(1000);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "");
                            }
                            if(DDS_mode==5)//ASK
                            {
                                //��ʾ
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "ASK Setted!");
                                GUI_Delay(1000);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "");
                            }
                            if(DDS_mode==6)//������VPP
                            {
                                //��ʾ
                                vpp = 100;//����Ĭ��100%
                                sprintf(buf, "%d", vpp);//��ʾ������С��
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "VPP Setted!");
                                GUI_Delay(1000);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "");
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_7: //����1
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 1;
                                sprintf(buf, "%d", num);//��ʾ�趨ֵ
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }

                            break;
                    }
                    break;
                case ID_BUTTON_8: //����2
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 2;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_9: //����3
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 3;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_10: //����4
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 4;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_11: //����5
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 5;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_12: //����6
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 6;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_13: //����7
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 7;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_14: //����8
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 8;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_15: //����9
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 9;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_16: //���C
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = 0;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_17: //����0
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 0;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_18: //�˸�<--
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num/10;
                                sprintf(buf, "%d", num);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            break;
                    }
                    break;
            }
            break;
        case WM_KEY_MSG://������Ϣ
            if(DDS_mode==1)//FRE
            {
                sprintf(buf, "%d", Frequency);
                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_11), buf);
            }
            if(DDS_mode==2)//AM
            {
                sprintf(buf, "%d", am);
                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_7), buf);
            }
            if(DDS_mode==3)//FM
            {
                sprintf(buf, "%d", fm*5);
                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_8), buf);
            }
            if(DDS_mode==6)//VPP
            {
                sprintf(buf, "%d", vpp);
                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
            }
            break;
        default:
            WM_DefaultProc(pMsg);
            break;
    }
}

//����һ���Ի���
WM_HWIN CreateFramewin(void)
{
    WM_HWIN hWin;

    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}

//��ں���
void emwinDemo(void)
{
    hWin_user=CreateFramewin();
//    while(1)
//    {
//        GUI_Delay(100);
//    }
}

