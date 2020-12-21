#include "DIALOG.h"
#include "emwinDemo.h"
#include <stdio.h>
#include "FFT.h"

extern float THD;
extern float time;
extern float wave_buf[FFT_LENGTH];;
extern float fft_outputbuf[FFT_LENGTH];

int auto_flag = 0;
int count = 0;

char buf[20];//������ʾ
GRAPH_DATA_Handle  pdataGRP1;
GRAPH_DATA_Handle  pdataGRP2;

WM_HWIN hWin_user;

#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)
#define ID_BUTTON_0     (GUI_ID_USER + 0x01)
#define ID_BUTTON_1     (GUI_ID_USER + 0x02)
#define ID_BUTTON_2     (GUI_ID_USER + 0x03)
#define ID_BUTTON_3     (GUI_ID_USER + 0x04)
#define ID_BUTTON_4     (GUI_ID_USER + 0x05)
#define ID_GRAPH_0      (GUI_ID_USER + 0x06)
#define ID_GRAPH_1      (GUI_ID_USER + 0x07)
#define ID_TEXT_0       (GUI_ID_USER + 0x08)
#define ID_TEXT_1       (GUI_ID_USER + 0x09)
#define ID_TEXT_2       (GUI_ID_USER + 0x0A)
#define ID_TEXT_3       (GUI_ID_USER + 0x0B)
#define ID_TEXT_4       (GUI_ID_USER + 0x0C)
#define ID_TEXT_5       (GUI_ID_USER + 0x0D)
#define ID_TEXT_6       (GUI_ID_USER + 0x0E)
#define ID_TEXT_7       (GUI_ID_USER + 0x0F)
#define ID_TEXT_8       (GUI_ID_USER + 0x10)
#define ID_TEXT_9       (GUI_ID_USER + 0x11)
#define ID_TEXT_10      (GUI_ID_USER + 0x12)
#define ID_TEXT_11      (GUI_ID_USER + 0x13)
#define ID_TEXT_12      (GUI_ID_USER + 0x14)
#define ID_TEXT_13      (GUI_ID_USER + 0x15)
#define ID_TEXT_14      (GUI_ID_USER + 0x16)
#define ID_TEXT_15      (GUI_ID_USER + 0x17)
#define ID_TEXT_16      (GUI_ID_USER + 0x18)
#define ID_BUTTON_5     (GUI_ID_USER + 0x19)

//�Ի�����Դ��
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
        { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x64, 0 },
        { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 80, 380, 230, 0, 0x0, 0 },
        { GRAPH_CreateIndirect, "Graph", ID_GRAPH_1, 420, 80, 380, 230, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 20, 360, 120, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 170, 360, 120, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_2, 320, 360, 120, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_3, 470, 360, 120, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_4, 620, 360, 120, 60, 0, 0x0, 0 },
        { BUTTON_CreateIndirect, "Button", ID_BUTTON_5, 700, 10, 60, 30, 0, 0x0, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_0, 30, 20, 100, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_1, 130, 20, 300, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_2, 400, 20, 100, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_3, 470, 20, 100, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_4, 10, 500, 200, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_5, 570, 60, 220, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_6, 730, 60, 100, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_7, 420, 80, 40, 40, 0, 0x64, 0 },

        { TEXT_CreateIndirect, "Text", ID_TEXT_8, 440, 120, 40, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_9, 475, 210, 40, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_10, 505, 240, 40, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_11, 540, 250, 40, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_12, 570, 260, 40, 40, 0, 0x64, 0 },

        { TEXT_CreateIndirect, "Text", ID_TEXT_13, 0, 60, 80, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_14, 420, 60, 80, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_15, 422, 296, 40, 40, 0, 0x64, 0 },
        { TEXT_CreateIndirect, "Text", ID_TEXT_16, 10, 80, 40, 40, 0, 0x64, 0 },

};

//�Ի���ص�����
static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    WM_HWIN hWin = pMsg->hWin;

    GRAPH_SCALE_Handle hScaleH;//ˮƽ����̶�
    GRAPH_SCALE_Handle hScaleV;//��ֱ����̶�

    GUI_POINT FFT_Point;

    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
            //��ʼ���Ի���
            hItem = pMsg->hWin;
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
            FRAMEWIN_SetText(hItem, "Amplifier nonlinear distortion research device");
            FRAMEWIN_SetFont(hItem, GUI_FONT_24_ASCII);

            //��ʼ��GRAPH
            hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
            GRAPH_SetBorder(hItem, 18, 0, 0, 0);
            GRAPH_SetGridVis(hItem, 1);//�����������
            GRAPH_SetGridFixedX(hItem, 1);//�̶� X �᷽�������
            GRAPH_SetGridDistY(hItem, 23);//���������� Y��
            GRAPH_SetGridDistX(hItem, 40);//X��

            hScaleV = GRAPH_SCALE_Create(18, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 46);//���������Ӵ�ֱ��Χ�߶ȱ�ǩ
            GRAPH_SCALE_SetFont(hScaleV, GUI_FONT_10_ASCII);//�������ڻ��ƿ̶ȱ�ŵ�����
            GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);//���ñ�ǩ������ɫ
            GRAPH_SCALE_SetFactor(hScaleV, 1.0/23);//�������ڼ���Ҫ���Ƶı�ŵ�����
            GRAPH_SCALE_SetNumDecs(hScaleV, 1);//С��λ��
            GRAPH_SCALE_SetOff(hScaleV, 115);//ƫ��
            GRAPH_AttachScale(hItem, hScaleV);//����ǩ��ӵ���ֱ����

            pdataGRP1 = GRAPH_DATA_XY_Create(GUI_LIGHTGREEN, 380/*������ݸ���*/, 0, 0);//����һ����������,�ɴ����������
            GRAPH_AttachData(hItem, pdataGRP1);//Ϊ��ͼ�ؼ�������ݶ���

            //��ʼ��GRAPH
            hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_1);
            GRAPH_SetBorder(hItem, 0, 0, 0, 10);

            hScaleH = GRAPH_SCALE_Create(220, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 40);//����������ˮƽ��Χ�߶ȱ�ǩ
            GRAPH_SCALE_SetFont(hScaleH, GUI_FONT_13_ASCII);//�������ڻ��ƿ̶ȱ�ŵ�����
            GRAPH_SCALE_SetFactor(hScaleH, 128.0/380);//�������ڼ���Ҫ���Ƶı�ŵ�����
            GRAPH_SCALE_SetTextColor(hScaleH, GUI_RED);//���ñ�ǩ������ɫ
            GRAPH_AttachScale(hItem, hScaleH);//��ӵ�ˮƽ����

            pdataGRP2 = GRAPH_DATA_XY_Create(GUI_LIGHTGREEN, 380/*������ݸ���*/, 0, 0);//����һ����������,�ɴ����������
            GRAPH_AttachData(hItem, pdataGRP2);//Ϊ��ͼ�ؼ�������ݶ���

            //��ʼ��BUTTON
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetText(hItem, "Normal");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
            BUTTON_SetText(hItem, "Top");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
            BUTTON_SetText(hItem, "Buttom");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
            BUTTON_SetText(hItem, "Two-way");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
            BUTTON_SetText(hItem, "Crossover");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
            BUTTON_SetTextColor(hItem,BUTTON_CI_UNPRESSED,GUI_RED);
            BUTTON_SetText(hItem, "Auto");
            BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);

            //��ʼ��TEXT
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetText(hItem, "Output:");
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
            TEXT_SetText(hItem, "THD:");
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
            TEXT_SetText(hItem, "Distortion:");
            TEXT_SetTextColor(hItem, GUI_BLUE);
            TEXT_SetFont(hItem, GUI_FONT_24_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
            TEXT_SetText(hItem, "Time for 512 Point FFT:");
            TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
            TEXT_SetText(hItem, "");
            TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
            TEXT_SetText(hItem, "");
            TEXT_SetTextColor(hItem, GUI_RED);
            TEXT_SetFont(hItem, GUI_FONT_13_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
            TEXT_SetText(hItem, "");
            TEXT_SetTextColor(hItem, GUI_MAGENTA);
            TEXT_SetFont(hItem, GUI_FONT_13B_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
            TEXT_SetText(hItem, "");
            TEXT_SetTextColor(hItem, GUI_MAGENTA);
            TEXT_SetFont(hItem, GUI_FONT_13B_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
            TEXT_SetText(hItem, "");
            TEXT_SetTextColor(hItem, GUI_MAGENTA);
            TEXT_SetFont(hItem, GUI_FONT_13B_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_11);
            TEXT_SetText(hItem, "");
            TEXT_SetTextColor(hItem, GUI_MAGENTA);
            TEXT_SetFont(hItem, GUI_FONT_13B_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_12);
            TEXT_SetText(hItem, "");
            TEXT_SetTextColor(hItem, GUI_MAGENTA);
            TEXT_SetFont(hItem, GUI_FONT_13B_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_13);
            TEXT_SetText(hItem, "Input:");
            TEXT_SetTextColor(hItem, GUI_LIGHTRED);
            TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_14);
            TEXT_SetText(hItem, "FFT Result:");
            TEXT_SetTextColor(hItem, GUI_LIGHTRED);
            TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_15);
            TEXT_SetText(hItem, "k");
            TEXT_SetTextColor(hItem, GUI_RED);
            TEXT_SetFont(hItem, GUI_FONT_16_ASCII);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_16);
            TEXT_SetText(hItem, "V");
            TEXT_SetTextColor(hItem, GUI_RED);
            TEXT_SetFont(hItem, GUI_FONT_13_ASCII);

            break;
        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            switch(Id)
            {
                case ID_BUTTON_0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED://��ť�����²��ͷ�
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), "Normal");
                            auto_flag = 0;
                            BUTTON_SetTextColor(WM_GetDialogItem(hWin,ID_BUTTON_5),BUTTON_CI_UNPRESSED,GUI_RED);
                            CONTROL_D1_HIGH;
                            CONTROL_D2_HIGH;
                            CONTROL_D3_LOW;
                            CONTROL_D4_HIGH;
                            CONTROL_D5_HIGH;
                            CONTROL_D6_LOW;
                            break;
                    }
                    break;
                case ID_BUTTON_1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED://��ť�����²��ͷ�
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), "Top Distortion");
                            auto_flag = 0;
                            BUTTON_SetTextColor(WM_GetDialogItem(hWin,ID_BUTTON_5),BUTTON_CI_UNPRESSED,GUI_RED);
                            CONTROL_D1_HIGH;
                            CONTROL_D2_LOW;
                            CONTROL_D3_LOW;
                            CONTROL_D4_HIGH;
                            CONTROL_D5_HIGH;
                            CONTROL_D6_LOW;
                            break;
                    }
                    break;
                case ID_BUTTON_2:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED://��ť�����²��ͷ�
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), "Bottom Distortion");
                            auto_flag = 0;
                            BUTTON_SetTextColor(WM_GetDialogItem(hWin,ID_BUTTON_5),BUTTON_CI_UNPRESSED,GUI_RED);
                            CONTROL_D1_LOW;
                            CONTROL_D2_LOW;
                            CONTROL_D3_LOW;
                            CONTROL_D4_HIGH;
                            CONTROL_D5_HIGH;
                            CONTROL_D6_LOW;
                            break;
                    }
                    break;
                case ID_BUTTON_3:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED://��ť�����²��ͷ�
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), "Two-way Distortion");
                            auto_flag = 0;
                            BUTTON_SetTextColor(WM_GetDialogItem(hWin,ID_BUTTON_5),BUTTON_CI_UNPRESSED,GUI_RED);
                            CONTROL_D1_LOW;
                            CONTROL_D2_LOW;
                            CONTROL_D3_LOW;
                            CONTROL_D4_LOW;
                            CONTROL_D5_HIGH;
                            CONTROL_D6_LOW;
                            break;
                    }
                    break;
                case ID_BUTTON_4:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED://��ť�����²��ͷ�
                            TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), "Crossover Distortion");
                            auto_flag = 0;
                            BUTTON_SetTextColor(WM_GetDialogItem(hWin,ID_BUTTON_5),BUTTON_CI_UNPRESSED,GUI_RED);
                            CONTROL_D1_HIGH;
                            CONTROL_D2_HIGH;
                            CONTROL_D3_LOW;
                            CONTROL_D4_HIGH;
                            CONTROL_D5_LOW;
                            CONTROL_D6_LOW;
                            break;
                    }
                    break;
                case ID_BUTTON_5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED://��ť�����²��ͷ�
                            if(auto_flag==0) {
                                auto_flag = 1;
                                BUTTON_SetTextColor(WM_GetDialogItem(hWin, ID_BUTTON_5), BUTTON_CI_UNPRESSED,
                                                    GUI_GREEN);
                                CONTROL_D1_HIGH;
                                CONTROL_D2_HIGH;
                                CONTROL_D3_LOW;
                                CONTROL_D4_HIGH;
                                CONTROL_D5_HIGH;
                                CONTROL_D6_LOW;
                                TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_1), "Normal");
                                count = 0;
                            }
                            else{
                                auto_flag = 0;
                                BUTTON_SetTextColor(WM_GetDialogItem(hWin, ID_BUTTON_5), BUTTON_CI_UNPRESSED,
                                                    GUI_RED);
                            }
                            break;
                    }
                    break;
            }
            break;
        case WM_SHOW_MSG://��ʾ
            sprintf(buf, "%0.3fms", time / 1000);
            TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_6), buf);

            if(THD<100) {
                sprintf(buf, "%.2lf %%", THD);
                TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_3), buf);

//                if (WM_IsCompletelyCovered(WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0))) break;//���л�������ҳ��ʲô������

                //������������ӵ���������
                for (int i = 0; i < 380; ++i) {
                    FFT_Point.x = i;
                    FFT_Point.y = (int16_t) (wave_buf[i]*23)+115;
                    GRAPH_DATA_XY_AddPoint(pdataGRP1, &FFT_Point);//��ֵ������
                }

                //��FFT������ӵ�Ƶ������
                for (int i = 1; i < FFT_LENGTH / 4; ++i) {
                    FFT_Point.x = i * 4;
                    if (fft_outputbuf[base_k] < 500)//���г��������Ӧ��Ƶ��
                    {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), "500");
                        FFT_Point.y = (int16_t) fft_outputbuf[i] / 3;
                    } else if (fft_outputbuf[base_k] < 600) {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), "600");
                        FFT_Point.y = fft_outputbuf[i] / 4;
                    } else if (fft_outputbuf[base_k] < 700) {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), "700");
                        FFT_Point.y = fft_outputbuf[i] / 4.5;
                    } else if (fft_outputbuf[base_k] < 800) {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), "800");
                        FFT_Point.y = fft_outputbuf[i] / 5;
                    } else if (fft_outputbuf[base_k] < 900) {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), "900");
                        FFT_Point.y = fft_outputbuf[i] / 5.5;
                    } else if (fft_outputbuf[base_k] < 1000) {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), "1000");
                        FFT_Point.y = fft_outputbuf[i] / 6;
                    } else {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), "1100");
                        FFT_Point.y = fft_outputbuf[i] / 6.5;
                    }

                    //���Ƶ��
                    if (fft_outputbuf[base_k] > 100) {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_8), "1kHz");
                    }
                    else{
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_8), "");
                    }
                    if (fft_outputbuf[base_k*2] > 4) {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_9), "2kHz");
                    }
                    else{
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_9), "");
                    }
                    if (fft_outputbuf[base_k*3] > 4) {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_10), "3kHz");
                    }
                    else{
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_10), "");
                    }
                    if (fft_outputbuf[base_k*4] > 4) {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_11), "4kHz");
                    }
                    else{
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_11), "");
                    }
                    if (fft_outputbuf[base_k*5] > 4) {
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_12), "5kHz");
                    }
                    else{
                        TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_12), "");
                    }

                    GRAPH_DATA_XY_AddPoint(pdataGRP2, &FFT_Point);//��ֵ������
                }
            }

            break;
        case WM_AUTO_MSG:
            if (auto_flag == 1)
            {
                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), "Normal");
            }
            if (auto_flag == 2)
            {
                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), "Top Distortion");
            }
            if (auto_flag == 3)
            {
                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), "Bottom Distortion");
            }
            if (auto_flag == 4)
            {
                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), "Two-way Distortion");
            }
            if (auto_flag == 5)
            {
                TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), "Crossover Distortion");
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
}

