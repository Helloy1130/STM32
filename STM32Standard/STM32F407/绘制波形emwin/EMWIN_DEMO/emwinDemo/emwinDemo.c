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

extern u16 sinValue[256]; //�����ź�
u16 i;

//�Ի�����Դ��
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

//�Ի���ص�����
static void _cbDialog(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	
	GRAPH_SCALE_Handle hScaleV;//��ֱ����̶�
	GRAPH_SCALE_Handle hScaleH;//ˮƽ����̶�
	
	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			//��ʼ���Ի���
			hItem = pMsg->hWin;
			FRAMEWIN_SetFont(hItem, GUI_FONT_24_ASCII);
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetText(hItem, "WAVE");
			//��ʼ��GRAPH
			hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
			GRAPH_SetBorder(hItem, 40, 30, 20, 40);
		    GRAPH_SetGridVis(hItem, 1);//�����������
			GRAPH_SetGridFixedX(hItem, 1);//�̶� X �᷽�������
			GRAPH_SetGridDistY(hItem, 35);//���������� Y��
			GRAPH_SetGridDistX(hItem, 40);//X��
			hScaleV = GRAPH_SCALE_Create(30, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 35);//���������Ӵ�ֱ��Χ�߶ȱ�ǩ
			GRAPH_SCALE_SetFont(hScaleV, GUI_FONT_16_ASCII);//�������ڻ��ƿ̶ȱ�ŵ�����
			GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);//���ñ�ǩ������ɫ
			GRAPH_SCALE_SetFactor(hScaleV, 1.0/70);//�������ڼ���Ҫ���Ƶı�ŵ�����
			GRAPH_SCALE_SetNumDecs(hScaleV, 1);//С��λ��
			//GRAPH_SCALE_SetOff(hScaleV, 140);//ƫ��
			GRAPH_AttachScale(hItem, hScaleV);//����ǩ��ӵ���ֱ����
			
			hScaleH = GRAPH_SCALE_Create(320, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 40);//����������ˮƽ��Χ�߶ȱ�ǩ
			GRAPH_AttachScale(hItem, hScaleH);//��ӵ�ˮƽ����
		    pdataGRP = GRAPH_DATA_YT_Create(GUI_GREEN, 640/*������ݸ���*/, 0, 0);//����һ����������,�ɴ���������� ͨ��������ʱ����ص�ͼ��
			GRAPH_AttachData(hItem, pdataGRP);//Ϊ��ͼ�ؼ�������ݶ���
			
			//��ʼ��TEXT	
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
			
			WM_CreateTimer(WM_GetClientWindow(pMsg->hWin), 1, 100, 0);//����һ�����������ʱ�� 100ms
			break;
		case WM_NOTIFY_PARENT:
			break;
		case WM_TIMER://��ʱ����Ϣ
			WM_RestartTimer(pMsg->Data.v, 1);//1ms
			if(WM_IsCompletelyCovered(WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0))) break;//���л�������ҳ��ʲô������

			//�����������ȡADCֵ
			GRAPH_DATA_YT_AddValue(pdataGRP, (I16)(sinValue[i]*3.3/4095*70));//��ֵ������
			i++;
			if(i==255) i = 0;
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

//demo����
void emwinDemo(void)
{
	hWin_user=CreateFramewin();
	while(1)
	{
		GUI_Delay(100);
	}
}

