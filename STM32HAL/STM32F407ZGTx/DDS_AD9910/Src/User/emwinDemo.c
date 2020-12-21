#include "DIALOG.h"
#include "emwinDemo.h"
#include "stdio.h"
#include "ad9910.h"

/*******************************************
DDS_mode:
			1:调频
			2:AM;
			3:FM;
			4:PSK
			5:ASK
			6:调节VPP
*******************************************/
uint8_t DDS_mode;

//频率
uint32_t Frequency;

//调节VPP
uint32_t vpp = 0;

char buf[10];//用于显示
uint32_t num;//当前设定值
uint32_t num2;//当前设定值

WM_HWIN hWin_user;

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
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
#define ID_TEXT_2 (GUI_ID_USER + 0x18)
#define ID_TEXT_3 (GUI_ID_USER + 0x19)
#define ID_TEXT_4 (GUI_ID_USER + 0x1A)
#define ID_TEXT_5 (GUI_ID_USER + 0x1B)
#define ID_TEXT_6 (GUI_ID_USER + 0x1C)
#define ID_TEXT_7 (GUI_ID_USER + 0x1D)
#define ID_TEXT_9 (GUI_ID_USER + 0x1F)
#define ID_TEXT_10 (GUI_ID_USER + 0x20)
#define ID_TEXT_11 (GUI_ID_USER + 0x21)
#define ID_TEXT_12 (GUI_ID_USER + 0x22)

//对话框资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
        { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 480, 800, 0, 0x64, 0 },
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
        { TEXT_CreateIndirect, "Text", ID_TEXT_2, 250, 50, 140, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_3, 10, 50, 150, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_4, 250, 90, 90, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_5, 10, 90, 140, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_6, 90, 10, 55, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_7, 330, 90, 60, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_9, 110, 50, 150, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_10, 390, 50, 50, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_11, 120, 90, 150, 20, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_12, 50, 130, 200, 20, 0, 0x64, 0 },
};

//对话框回调函数
static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    WM_HWIN hWin = pMsg->hWin;

    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
            //初始化对话框
            hItem = pMsg->hWin;
            FRAMEWIN_SetText(hItem, "DDS");
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetFont(hItem, GUI_FONT_24_ASCII);
            //初始化BUTTON
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
            //初始化TEXT
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetText(hItem, "MODE:");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
            TEXT_SetText(hItem, "SET VPP(mV):");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
            TEXT_SetText(hItem, "SET FRE(Hz):");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
            TEXT_SetText(hItem, "VPP(mV):");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
            TEXT_SetText(hItem, "Frequency(Hz):");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

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
                            if(DDS_mode==1) //调频
                            {
                                if(num<100000000)//100M以上设置失败
                                {
                                    //显示
                                    Frequency  = num;
                                    sprintf(buf, "%d", Frequency);//显示频率
                                    Freq_Amp_set(Frequency,vpp);//设置频率，幅度（幅度假设上限为1，下限为0，AD9910实际输出幅度上限约为800毫伏左右）
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
                            if(DDS_mode==6)//调幅度VPP
                            {
                                //显示
                                vpp = num2;
                                sprintf(buf, "%d", vpp);
                                Freq_Amp_set(Frequency,vpp/20.0*0.83);//设置频率，幅度（幅度假设上限为1，下限为0，AD9910实际输出幅度上限约为800毫伏左右）
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_7), buf);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "VPP Setted!");
                                GUI_Delay(1000);
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "");
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_7: //数字1
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            if(DDS_mode==1)
                            {
                                num = num * 10 + 1;
                                sprintf(buf, "%d", num);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), buf);
                            }
                            if(DDS_mode==6)
                            {
                                num2 = num2 * 10 + 1;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }

                            break;
                    }
                    break;
                case ID_BUTTON_8: //数字2
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
                            if(DDS_mode==6)
                            {
                                num2 = num2 * 10 + 2;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_9: //数字3
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
                            if(DDS_mode==6)
                            {
                                num2 = num2 * 10 + 3;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_10: //数字4
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
                            if(DDS_mode==6)
                            {
                                num2 = num2 * 10 + 4;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_11: //数字5
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
                            if(DDS_mode==6)
                            {
                                num2 = num2 * 10 + 5;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_12: //数字6
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
                            if(DDS_mode==6)
                            {
                                num2 = num2 * 10 + 6;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_13: //数字7
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
                            if(DDS_mode==6)
                            {
                                num2 = num2 * 10 + 7;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_14: //数字8
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
                            if(DDS_mode==6)
                            {
                                num2 = num2 * 10 + 8;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_15: //数字9
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
                            if(DDS_mode==6)
                            {
                                num2 = num2 * 10 + 9;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_16: //清空C
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
                            if(DDS_mode==6)
                            {
                                num2 = 0;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_17: //数字0
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
                            if(DDS_mode==6)
                            {
                                num2 = num2 * 10 + 0;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
                case ID_BUTTON_18: //退格<--
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
                            if(DDS_mode==6)
                            {
                                num2 = num2/10;
                                sprintf(buf, "%d", num2);//显示设定值
                                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
                            }
                            break;
                    }
                    break;
            }
            break;
        default:
            WM_DefaultProc(pMsg);
            break;
    }
}

//创建一个对话框
WM_HWIN CreateFramewin(void)
{
    WM_HWIN hWin;

    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}

//入口函数
void emwinDemo(void)
{
    hWin_user=CreateFramewin();
}

