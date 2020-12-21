#include "DIALOG.h"
#include "emwinDemo.h"
#include "stdio.h"

extern double frequency;//频率
extern double period;//周期
extern double delay_time;//时间间隔
extern double duty;//占空比
char buf[20];//用于显示

WM_HWIN hWin_user;

#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)
#define ID_BUTTON_0   (GUI_ID_USER + 0x01)
#define ID_TEXT_0   (GUI_ID_USER + 0x03)
#define ID_TEXT_1   (GUI_ID_USER + 0x04)
#define ID_TEXT_2   (GUI_ID_USER + 0x05)
#define ID_TEXT_3   (GUI_ID_USER + 0x06)
#define ID_TEXT_4   (GUI_ID_USER + 0x07)
#define ID_TEXT_5   (GUI_ID_USER + 0x08)
#define ID_TEXT_6   (GUI_ID_USER + 0x09)
#define ID_TEXT_7   (GUI_ID_USER + 0x0A)
#define ID_TEXT_8   (GUI_ID_USER + 0x0B)

//对话框资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
        {
                { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 480, 800, 0, 0x64, 0 },
                { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 120, 465, 200, 100, 0, 0x0, 0 },
                { TEXT_CreateIndirect, "Text", ID_TEXT_0, 5, 150, 120, 40, 0, 0x64, 0 },
                { TEXT_CreateIndirect, "Text", ID_TEXT_1, 5, 210, 80, 40, 0, 0x64, 0 },
                { TEXT_CreateIndirect, "Text", ID_TEXT_2, 5, 270, 66, 40, 0, 0x64, 0 },
                { TEXT_CreateIndirect, "Text", ID_TEXT_3, 5, 330, 155, 40, 0, 0x64, 0 },
                { TEXT_CreateIndirect, "Text", ID_TEXT_4, 160, 150, 150, 40, 0, 0x64, 0 },
                { TEXT_CreateIndirect, "Text", ID_TEXT_5, 160, 210, 150, 40, 0, 0x64, 0 },
                { TEXT_CreateIndirect, "Text", ID_TEXT_6, 160, 270, 150, 40, 0, 0x64, 0 },
                { TEXT_CreateIndirect, "Text", ID_TEXT_7, 160, 330, 150, 40, 0, 0x64, 0 },
                { TEXT_CreateIndirect, "Text", ID_TEXT_8, 0, 0, 472, 79, 0, 0x64, 0 },
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
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetText(hItem, "Digital Frequency Meter");
            FRAMEWIN_SetFont(hItem, GUI_FONT_24_ASCII);
            //初始化BUTTON
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetText(hItem, "START");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
            //初始化TEXT
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetText(hItem, "Frequency:");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
            TEXT_SetText(hItem, "Period:");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetText(hItem, "Duty:");

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
            TEXT_SetText(hItem, "TimeDelay(A-B):");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);//频率
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetText(hItem, "");

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);//周期
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);//占空比
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);//时间间隔
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetText(hItem, "");

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);//提示信息
            TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
            TEXT_SetTextColor(hItem, GUI_BLUE);
            break;
        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            switch(Id)
            {
                case ID_BUTTON_0: //开启闸门1次
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED://按钮被按下并释放
                            GATE_HIGH;//发送高电平
                            GUI_Delay(1);//1ms
                            GATE_LOW;//拉低恢复

                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_4), "");
							TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_8), "Start!");
							GUI_Delay(500);//500ms
							TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_8), "");
                            break;
                    }
                    break;
            }
            break;
        case WM_SHOW_MSG://显示
            //显示频率和周期
            if(frequency > 0)
            {
                if(frequency < 1000)//Hz ms
                {
                    sprintf(buf, "%.4lf Hz", frequency);
                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_4), buf);
                    if(frequency <= 1)//s
                    {
                        sprintf(buf, "%.4lf s", period);
                        TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_5), buf);
                    }
                    else//ms
                    {
                        sprintf(buf, "%.4lf ms", period * 1000);
                        TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_5), buf);
                    }
                }
                else if(frequency < 1000000)//KHz us
                {
                    sprintf(buf, "%.4lf KHz", frequency/1000);
                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_4), buf);
                    if(frequency == 1000)//ms
                    {
                        sprintf(buf, "%.4lf ms", period * 1000);
                        TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_5), buf);
                    }
                    else//us
                    {
                        sprintf(buf, "%.4lf us", period * 1000000);
                        TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_5), buf);
                    }
                }
                else//MHz
                {
                    sprintf(buf, "%.6lf MHz", frequency/1000000);
                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_4), buf);
                    if(frequency == 1000000)//us
                    {
                        sprintf(buf, "%.4lf us", period * 1000000);
                        TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_5), buf);
                    }
                    else//ns
                    {
                        sprintf(buf, "%.4lf ns", period * 1000000000);
                        TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_5), buf);
                    }
                }
                frequency = 0;
            }
            //显示占空比
            if(duty > 0)
            {
                sprintf(buf, "%.1lf %%", duty);
                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_6), buf);
                duty = 0;
            }
            //显示时间间隔
            if(delay_time > 0)
            {
                if(delay_time >= 0.001)//ms
                {
                    sprintf(buf, "%.2lf ms", delay_time * 1000);
                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_7), buf);
                }
                else if(delay_time >= 0.000001)//us
                {
                    sprintf(buf, "%.2lf us", delay_time * 1000000);
                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_7), buf);
                }
                else//ns
                {
                    sprintf(buf, "%.2lf ns", delay_time * 1000000000);
                    TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_7), buf);
                }
                delay_time = 0;
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

