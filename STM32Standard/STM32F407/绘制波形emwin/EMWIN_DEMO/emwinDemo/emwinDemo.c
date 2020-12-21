#include "DIALOG.h"
#include "emwinDemo.h"
#include "stdio.h"

WM_HWIN hWin_user;

#define ID_FRAMEWIN_0   	(GUI_ID_USER + 0x00)
#define ID_GRAPH_0         	(GUI_ID_USER + 0x01)
#define ID_TEXT_0           (GUI_ID_USER + 0x02)
#define ID_TEXT_1           (GUI_ID_USER + 0x03)
#define ID_TEXT_2           (GUI_ID_USER + 0x04)
#define ID_TEXT_3           (GUI_ID_USER + 0x05)
#define ID_TEXT_4           (GUI_ID_USER + 0x06)
#define ID_TEXT_5           (GUI_ID_USER + 0x07)

GRAPH_DATA_Handle  pdataGRP;

extern u16 sinValue[256]; //正弦信号
u16 i;

//对话框资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x64, 0 },
	{ GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 0, 700, 360, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_0, 272, 8, 80, 20, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_1, 9, 380, 122, 29, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_2, 132, 384, 148, 25, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_3, 331, 383, 80, 27, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_4, 402, 381, 99, 29, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_5, 30, 10, 40, 20, 0, 0x64, 0 },
};

//对话框回调函数
static void _cbDialog(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	
	GRAPH_SCALE_Handle hScaleV;//垂直方向刻度
	GRAPH_SCALE_Handle hScaleH;//水平方向刻度
	
	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			//初始化对话框
			hItem = pMsg->hWin;
			FRAMEWIN_SetFont(hItem, GUI_FONT_24_ASCII);
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetText(hItem, "WAVE");
			//初始化GRAPH
			hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
			GRAPH_SetBorder(hItem, 40, 30, 20, 40);
		    GRAPH_SetGridVis(hItem, 1);//启动网格绘制
			GRAPH_SetGridFixedX(hItem, 1);//固定 X 轴方向的网格
			GRAPH_SetGridDistY(hItem, 35);//设置网格间距 Y轴
			GRAPH_SetGridDistX(hItem, 40);//X轴
			hScaleV = GRAPH_SCALE_Create(30, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 35);//创建和增加垂直范围尺度标签
			GRAPH_SCALE_SetFont(hScaleV, GUI_FONT_16_ASCII);//设置用于绘制刻度编号的字体
			GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);//设置标签字体颜色
			GRAPH_SCALE_SetFactor(hScaleV, 1.0/70);//设置用于计算要绘制的编号的因子
			GRAPH_SCALE_SetNumDecs(hScaleV, 1);//小数位数
			//GRAPH_SCALE_SetOff(hScaleV, 140);//偏移
			GRAPH_AttachScale(hItem, hScaleV);//将标签添加到垂直方向
			
			hScaleH = GRAPH_SCALE_Create(320, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 40);//创建和增加水平范围尺度标签
			GRAPH_AttachScale(hItem, hScaleH);//添加到水平方向
		    pdataGRP = GRAPH_DATA_YT_Create(GUI_GREEN, 640/*最大数据个数*/, 0, 0);//创建一个数据曲线,可创建多个曲线 通常用于与时间相关的图形
			GRAPH_AttachData(hItem, pdataGRP);//为绘图控件添加数据对象
			
			//初始化TEXT	
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
			TEXT_SetText(hItem, "WAVE");
			TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetTextColor(hItem, GUI_RED);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
			TEXT_SetText(hItem, "Frequency:");
			TEXT_SetTextColor(hItem, GUI_BLUE);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
			TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetText(hItem, "1KHz");

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
			TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetText(hItem, "VPP:");
			TEXT_SetTextColor(hItem, GUI_BLUE);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
			TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetText(hItem, "2V");
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
			TEXT_SetText(hItem, "V");
			TEXT_SetFont(hItem, GUI_FONT_20_ASCII);
			TEXT_SetTextColor(hItem, GUI_RED);
			
			WM_CreateTimer(WM_GetClientWindow(pMsg->hWin), 1, 100, 0);//创建一个单次软件定时器 100ms
			break;
		case WM_NOTIFY_PARENT:
			break;
		case WM_TIMER://定时器消息
			WM_RestartTimer(pMsg->Data.v, 1);//1ms
			if(WM_IsCompletelyCovered(WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0))) break;//当切换到其他页面什么都不做

			//可以在这里获取ADC值
			GRAPH_DATA_YT_AddValue(pdataGRP, (I16)(sinValue[i]*3.3/4095*70));//赋值到曲线
			i++;
			if(i==255) i = 0;
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

//demo函数
void emwinDemo(void)
{
	hWin_user=CreateFramewin();
	while(1)
	{
		GUI_Delay(100);
	}
}

