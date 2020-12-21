#include "DIALOG.h"
#include "emwinDemo.h"
#include "stdio.h"
#include "gpio.h"
#include "ad9910.h"

//模式
//1：检测输入电阻 2：检测输出电阻 3：检测增益 4：检测幅频特性 5：故障检测
u8 mode;
u8 ADC_flag=0;

extern u16 Ri; 
extern u16 Ro;
extern double Av;
extern u32 freq_3dbH;

extern u8 currentState;//1代表没出故障，0代表出故障
extern u8 testResult;

char buf[20];//用于显示

WM_HWIN hWin_user;

#define ID_FRAMEWIN_0    (GUI_ID_USER + 0x00)
#define ID_GRAPH_0    (GUI_ID_USER + 0x01)
#define ID_BUTTON_0    (GUI_ID_USER + 0x02)
#define ID_BUTTON_1    (GUI_ID_USER + 0x03)
#define ID_BUTTON_2    (GUI_ID_USER + 0x04)
#define ID_BUTTON_3    (GUI_ID_USER + 0x05)
#define ID_BUTTON_4    (GUI_ID_USER + 0x06)
#define ID_TEXT_0    (GUI_ID_USER + 0x07)
#define ID_TEXT_1    (GUI_ID_USER + 0x08)
#define ID_TEXT_2    (GUI_ID_USER + 0x09)
#define ID_TEXT_3    (GUI_ID_USER + 0x0A)
#define ID_TEXT_4    (GUI_ID_USER + 0x0B)
#define ID_TEXT_5    (GUI_ID_USER + 0x0C)
#define ID_TEXT_6    (GUI_ID_USER + 0x0D)
#define ID_TEXT_7    (GUI_ID_USER + 0x0E)
#define ID_TEXT_8    (GUI_ID_USER + 0x0F)
#define ID_TEXT_9    (GUI_ID_USER + 0x10)
#define ID_TEXT_10    (GUI_ID_USER + 0x11)
#define ID_TEXT_11    (GUI_ID_USER + 0x12)
#define ID_TEXT_12    (GUI_ID_USER + 0x13)

GRAPH_DATA_Handle  pdataGRP;//绘图数据对象

//对话框资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x64, 0 },
    { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 0, 550, 360, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 0, 365, 120, 80, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 158, 365, 120, 80, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_2, 320, 365, 120, 80, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_3, 476, 365, 120, 80, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button", ID_BUTTON_4, 633, 365, 130, 80, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_0, 148, 7, 262, 25, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_1, 14, 12, 35, 20, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_2, 510, 325, 51, 20, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_3, 567, 13, 39, 24, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_4, 609, 12, 154, 24, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_5, 567, 52, 40, 24, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_6, 608, 52, 161, 26, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_7, 569, 89, 42, 28, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_8, 607, 89, 172, 26, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_9, 571, 126, 35, 26, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_10, 612, 125, 160, 26, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_11, 571, 185, 174, 27, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_12, 589, 236, 110, 27, 0, 0x64, 0 },
};

//对话框回调函数
static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;

    GRAPH_SCALE_Handle hScaleV;//垂直方向刻度
    GRAPH_SCALE_Handle hScaleH;//水平方向刻度

    WM_HWIN hWin = pMsg->hWin;

    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        //初始化对话框
        hItem = pMsg->hWin;
        FRAMEWIN_SetText(hItem, "Circuit Tester");
        FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
        FRAMEWIN_SetFont(hItem, GUI_FONT_20_ASCII);
        //初始化曲线
        hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
        GRAPH_SetBorder(hItem, 30, 30, 20, 30);//宽550-50=500  高360-60=300
        GRAPH_SetGridVis(hItem, 1);//启动网格绘制
        GRAPH_SetGridFixedX(hItem, 1);//固定 X 轴方向的网格
        GRAPH_SetGridDistY(hItem, 35);//设置网格间距 Y轴
        GRAPH_SetGridDistX(hItem, 40);//X轴

        hScaleV = GRAPH_SCALE_Create(30, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 35);//创建和增加垂直范围尺度标签
        GRAPH_SCALE_SetFont(hScaleV, GUI_FONT_16_ASCII);//设置用于绘制刻度编号的字体
        GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);//设置标签字体颜色
        GRAPH_SCALE_SetFactor(hScaleV, 1.0/10);//设置用于计算要绘制的编号的因子
        GRAPH_SCALE_SetNumDecs(hScaleV, 1);//小数位数
        GRAPH_AttachScale(hItem, hScaleV);//将标签添加到垂直方向

        hScaleH = GRAPH_SCALE_Create(340, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 40);//创建和增加水平范围尺度标签
        GRAPH_SCALE_SetFont(hScaleH, GUI_FONT_16_ASCII);//设置用于绘制刻度编号的字体
        GRAPH_SCALE_SetTextColor(hScaleH, GUI_RED);//设置标签字体颜色
		GRAPH_SCALE_SetFactor(hScaleV, 10);//设置用于计算要绘制的编号的因子
        GRAPH_AttachScale(hItem, hScaleH);//添加到水平方向

        pdataGRP = GRAPH_DATA_XY_Create(GUI_GREEN, 640/*最大数据个数*/, 0, 0);//创建一个数据曲线,可创建多个曲线
        GRAPH_AttachData(hItem, pdataGRP);//为绘图控件添加数据对象
        //初始化BUTTON
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetText(hItem, "Ri");
        BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetText(hItem, "Ro");
        BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
        BUTTON_SetText(hItem, "Av");

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
        BUTTON_SetText(hItem, "Am-Fre");
        BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
        BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
        BUTTON_SetText(hItem, "Fault-Analyze");
        //初始化TEXT
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
        TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
        TEXT_SetFont(hItem, GUI_FONT_20_ASCII);
        TEXT_SetText(hItem, "Amplitude-frequency curve");
        TEXT_SetTextColor(hItem, GUI_RED);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
        TEXT_SetFont(hItem, GUI_FONT_16_ASCII);
        TEXT_SetText(hItem, "Am/V");
        TEXT_SetTextColor(hItem, GUI_RED);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
        TEXT_SetFont(hItem, GUI_FONT_16_ASCII);
        TEXT_SetTextColor(hItem, GUI_RED);
        TEXT_SetText(hItem, "Fre/Hz");

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetText(hItem, "Ri:");
        TEXT_SetTextColor(hItem, GUI_BLUE);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
        TEXT_SetText(hItem, "50k Ohm");
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetText(hItem, "Ro:");
        TEXT_SetTextColor(hItem, GUI_BLUE);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetText(hItem, "500 Ohm");

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetText(hItem, "Av:");
        TEXT_SetTextColor(hItem, GUI_BLUE);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
        TEXT_SetText(hItem, "129.1 V/V");
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetText(hItem, "fH:");
        TEXT_SetTextColor(hItem, GUI_BLUE);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
        TEXT_SetText(hItem, "59.1 kHz");
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_11);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_RED);
        TEXT_SetText(hItem, "Circuit is normal!");

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_12);
        TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
        TEXT_SetTextColor(hItem, GUI_RED);
        TEXT_SetText(hItem, "R2 Open!");
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch(Id)
        {
        case ID_BUTTON_0://测量输入电阻
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED://按钮被按下并释放
				//设定DDS参数
				Freq_convert(1000);//1kHz
				Write_Amplitude(20);//20mV
				//继电器控制采样电阻接入电路 空载 测输入
				Relay_inout = 0;
				Relay_RS = 0;
				Relay_RL = 0;
				//给FPGA发送命令开始检测
				startSignal = 1;
				mode = 1;
                break;
            }
            break;
        case ID_BUTTON_1://测量输出电阻
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED://按钮被按下并释放
				//设定DDS参数
				Freq_convert(1000);//1kHz
				Write_Amplitude(20);//20mV
				//继电器控制采样电阻短路 测输出  空载 
				Relay_inout = 1;
				Relay_RS = 1;
				Relay_RL = 0;
				//给FPGA发送命令开始检测
				startSignal = 1;
				mode = 2;
                break;
            }
            break;
        case ID_BUTTON_2://测量增益
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED://按钮被按下并释放
				//设定DDS参数
				Freq_convert(1000);//1kHz
				Write_Amplitude(20);//20mV
				//继电器控制采样电阻短路 测输出  空载 
				Relay_inout = 1;
				Relay_RS = 1;
				Relay_RL = 0;			
				//给FPGA发送命令开始检测
				startSignal = 1;
				mode = 3;
                break;
            }
            break;
        case ID_BUTTON_3://测量幅频特性
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED://按钮被按下并释放
				//设定DDS参数
				Write_Amplitude(20);//20mV
				//继电器控制采样电阻短路 测输出  空载 
				Relay_inout = 1;
				Relay_RS = 1;
				Relay_RL = 0;
				
				mode = 4;
				ADC_flag = 1;
                break;
            }
            break;
        case ID_BUTTON_4://开启故障检测
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED://按钮被按下并释放
				mode = 5;
				currentState = 1;
				TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_11), "Circuit is normal!");
				TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "");
                break;
            }
            break;
        }
        break;
    case WM_SHOW_MSG://显示
		    switch(mode)
            {
				case 0:
					break;
				case 1://显示输入电阻
					sprintf(buf, "%d Ohm", Ri);
					TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_4), buf);
					break;
				case 2://显示输出电阻
					sprintf(buf, "%d Ohm", Ro);
					TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_6), buf);
					break;
				case 3://显示增益
					sprintf(buf, "%.1f V/V", Av);
					TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_8), buf);
					break;
				case 4://显示-3db上限频率值
					sprintf(buf, "%.1f KHz", freq_3dbH/1000.0);
					TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_10), buf);
					break;
				case 5://显示故障
					if(currentState)
					{
						TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_11), "Circuit is normal!");
						TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "");
					}
					else
					{
						TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_11), "Circuit failure!");
						switch(testResult)
						{
							case  1://R1短路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "R1 Short!");
								break;
							case  2://R2短路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "R2 Short!");
								break;
							case  3://R3短路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "R3 Short!");
								break;
							case  4://R4短路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "R4 Short!");
								break;
							case  5://R1开路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "R1 Open!");
								break;
							case  6://R2开路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "R2 Open!");
								break;
							case  7://R3开路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "R3 Open!");
								break;
							case  8://R4开路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "R4 Open!");
								break;
							case  9://C1开路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "C1 Open!");
								break;
							case 10://C2开路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "C2 Open!");
								break;
							case 11://C3开路
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "C3 Open!");
								break;
							case 12://C1两倍
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "C1 Double!");
								break;
							case 13://C2两倍
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "C2 Double!");
								break;
							case 14://C3两倍
								TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), "C3 Double!");
								break;
						}
					}
					
					TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_12), buf);
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

//演示函数
void emwinDemo(void)
{
    hWin_user=CreateFramewin();
    while(1)
    {
        GUI_Delay(100);
    }
}

