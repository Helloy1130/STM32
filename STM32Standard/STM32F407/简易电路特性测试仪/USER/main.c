#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "sram.h"
#include "malloc.h"
#include "ILI93xx.h"
#include "led.h"
#include "timer.h"
#include "touch.h"
#include "GUI.h"
#include "w25qxx.h"
#include "includes.h"
#include "emwinDemo.h"
#include "WM.h"
#include "DIALOG.h"
#include "spi.h"
#include "ad9910.h"
#include "adc.h"
#include "gpio.h"

#define RS 15000//��������RS ��λŷķ
#define RL 1000//����RL ��λŷķ
#define FACTOR1 334.33//INA����=1+50K/150=334.33
#define FACTOR2 1.7259//ʵ���ѹ˥��Ϊ0.5794�� 1.7259=1/0.5794

extern WM_HWIN hWin_user;//�Ի���
extern GRAPH_DATA_Handle  pdataGRP;//��ͼ����

extern u8 end_flag;
extern u16 outputData[20];//�洢��⵽���������

extern u8 mode;
extern u8 ADC_flag;

u32 freq = 10;

u16 Ri;
u16 Ro;
double Av;
double VH;//�����ĵ�·���޽�ֹƵ�ʶ�Ӧ��ֵ
double VL;//�����ĵ�·���޽�ֹƵ�ʶ�Ӧ��ֵ
u32 freq_3dbH;//���޽�ֹpl
u32 freq_3dbL;//���޽�ֹpl

//1����û�����ϣ�0���������
u8 currentState;
//1:��������
//2�����������
//3����������
//4�����Ƶ
u8 test;
//1-4 R1-R4��·
//5-8 R1-R4��·
//9-11 C1-C3��·
//12-14 C1-C3����
u8 testResult;

//�������ȼ�
#define START_TASK_PRIO				3
//�����ջ��С
#define START_STK_SIZE 				1024
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//TOUCH����
//�����������ȼ�
#define TOUCH_TASK_PRIO				4
//�����ջ��С
#define TOUCH_STK_SIZE				128
//������ƿ�
OS_TCB TouchTaskTCB;
//�����ջ
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch����
void touch_task(void *p_arg);

//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO 				5
//�����ջ��С
#define LED0_STK_SIZE				128
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0����
void led0_task(void *p_arg);

//EMWINDEMO����
//�����������ȼ�
#define EMWINDEMO_TASK_PRIO			6
//�����ջ��С
#define EMWINDEMO_STK_SIZE			2048
//������ƿ�
OS_TCB EmwindemoTaskTCB;
//�����ջ
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task����
void emwindemo_task(void *p_arg);

//������������
//�����������ȼ�
#define RECEIVE_TASK_PRIO 			7
//�����ջ��С
#define RECEIVE_STK_SIZE			128
//������ƿ�
OS_TCB ReceiveTaskTCB;
//�����ջ
CPU_STK RECEIVE_TASK_STK[RECEIVE_STK_SIZE];
//receive����
void receive_task(void *p_arg);

//��Ƶ���Բ�������
//�����������ȼ�
#define AMFRE_TASK_PRIO 			8
//�����ջ��С
#define AMFRE_STK_SIZE				128
//������ƿ�
OS_TCB AmfreTaskTCB;
//�����ջ
CPU_STK AMFRE_TASK_STK[AMFRE_STK_SIZE];
//amfre����
void amfre_task(void *p_arg);

//���ϼ������
//�����������ȼ�
#define TEST_TASK_PRIO 				9
//�����ջ��С
#define TEST_STK_SIZE				128
//������ƿ�
OS_TCB TestTaskTCB;
//�����ջ
CPU_STK TEST_TASK_STK[TEST_STK_SIZE];
//test����
void test_task(void *p_arg);

int main(void)
{
    OS_ERR err;
    CPU_SR_ALLOC();

    delay_init(168);       	//��ʱ��ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//�жϷ�������
    uart_init(115200);    	//���ڲ���������
    TFTLCD_Init();			//��ʼ��LCD
	W25QXX_Init();			//��ʼ��W25Q128

    LED_Init();   			//LED��ʼ��
    MySPI_Init();			//SPI�����ʼ��
    AD9910_Init();			//AD9910��ʼ��
    Adc_Init();				//Ƭ��ADC��ʼ��
    myGPIO_init();			//�Զ����GPIO

    //��ʼ״̬��1kHz��Vpp��20mV
    Freq_convert(1000); //дƵ��
    Write_Amplitude(20); //д����

    FSMC_SRAM_Init(); 		//SRAM��ʼ��
    mem_init(SRAMIN); 		//�ڲ�RAM��ʼ��
    mem_init(SRAMEX); 		//�ⲿRAM��ʼ��
    mem_init(SRAMCCM);		//CCM��ʼ��

    TP_Init();				//��ʼ��������
    OSInit(&err);			//��ʼ��UCOSIII
    OS_CRITICAL_ENTER();	//�����ٽ���
    //������ʼ����
    OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
                 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
    OS_CRITICAL_EXIT();	//�˳��ٽ���
    OSStart(&err);  //����UCOSIII
    while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    p_arg = p_arg;

    CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);  	//ͳ������
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
    //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
    OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);
#endif
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//����CRCʱ��
    WM_SetCreateFlags(WM_CF_MEMDEV); //�������д��ڵĴ洢�豸
    GUI_Init();  			//STemWin��ʼ��

    OS_CRITICAL_ENTER();	//�����ٽ���
    //STemWin Demo����
    OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,
                 (CPU_CHAR*   )"Emwindemo task",
                 (OS_TASK_PTR )emwindemo_task,
                 (void*       )0,
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void*       )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
    //����������
    OSTaskCreate((OS_TCB*     )&TouchTaskTCB,
                 (CPU_CHAR*   )"Touch task",
                 (OS_TASK_PTR )touch_task,
                 (void*       )0,
                 (OS_PRIO	  )TOUCH_TASK_PRIO,
                 (CPU_STK*    )&TOUCH_TASK_STK[0],
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void*       )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
    //LED0����
    OSTaskCreate((OS_TCB*     )&Led0TaskTCB,
                 (CPU_CHAR*   )"Led0 task",
                 (OS_TASK_PTR )led0_task,
                 (void*       )0,
                 (OS_PRIO	  )LED0_TASK_PRIO,
                 (CPU_STK*    )&LED0_TASK_STK[0],
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,
                 (CPU_STK_SIZE)LED0_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void*       )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
    //������������
    OSTaskCreate((OS_TCB*     )&ReceiveTaskTCB,
                 (CPU_CHAR*   )"Receive task",
                 (OS_TASK_PTR )receive_task,
                 (void*       )0,
                 (OS_PRIO	  )RECEIVE_TASK_PRIO,
                 (CPU_STK*    )&RECEIVE_TASK_STK[0],
                 (CPU_STK_SIZE)RECEIVE_STK_SIZE/10,
                 (CPU_STK_SIZE)RECEIVE_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void*       )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
    //��Ƶ���Բ�������
    OSTaskCreate((OS_TCB*     )&AmfreTaskTCB,
                 (CPU_CHAR*   )"Amfre task",
                 (OS_TASK_PTR )amfre_task,
                 (void*       )0,
                 (OS_PRIO	  )AMFRE_TASK_PRIO,
                 (CPU_STK*    )&AMFRE_TASK_STK[0],
                 (CPU_STK_SIZE)AMFRE_STK_SIZE/10,
                 (CPU_STK_SIZE)AMFRE_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void*       )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
    //���ϼ������
    OSTaskCreate((OS_TCB*     )&TestTaskTCB,
                 (CPU_CHAR*   )"Test task",
                 (OS_TASK_PTR )test_task,
                 (void*       )0,
                 (OS_PRIO	  )TEST_TASK_PRIO,
                 (CPU_STK*    )&TEST_TASK_STK[0],
                 (CPU_STK_SIZE)TEST_STK_SIZE/10,
                 (CPU_STK_SIZE)TEST_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void*       )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
    OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����
    OS_CRITICAL_EXIT();	//�˳��ٽ���
}

//EMWINDEMO����
void emwindemo_task(void *p_arg)
{
    //����Ƥ��
    BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
    CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
    DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
    FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
    HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
    MENU_SetDefaultSkin(MENU_SKIN_FLEX);
    MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
    PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
    RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
    SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
    SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
    SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);
    while(1)
    {
        emwinDemo();
    }
}

//TOUCH����
void touch_task(void *p_arg)
{
    OS_ERR err;
    while(1)
    {
        GUI_TOUCH_Exec();
        OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
    }
}

//LED0����
void led0_task(void *p_arg)
{
    OS_ERR err;
    while(1)
    {
        LED0 = !LED0;
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
    }
}

//������������
void receive_task(void *p_arg)
{
    u8 flag = 0;//����
    u8 i;
    u16 dataMax=0;
    u16 dataMin=65535;
    double Vpp1=0;
    double Vpp2=0;
    OS_ERR err;

    WM_MESSAGE message;
    message.MsgId = WM_SHOW_MSG;

    while(1)
    {
        if(startSignal)
        {
            if(MySPI_ReadByte() == 0x11)
            {
                //printf("1\r\n");
                if(end_flag == 0xff)
                {
                    //���ݴ���
                    if(mode==1)//����������
                    {
                        startSignal = 0;//������
                        //���ݴ���
                        for(i=0; i<20; i++)
                        {
                            if(outputData[i]>dataMax)
                            {
                                dataMax = outputData[i];
                            }
                            if(outputData[i]<dataMin)
                            {
                                dataMin = outputData[i];
                            }
                        }
                        Vpp1 = (dataMax - dataMin)*20*1000/65535.0/FACTOR1;//�����ѹ ��λmV �������20V
                        Ri = (20-Vpp1)*RS/Vpp1;//�õ��������
                        dataMax=0;
                        dataMin=65535;

                        WM_SendMessage(WM_GetClientWindow(hWin_user), &message);//������Ϣ������,��ʾ
                    }
                    if(mode==2)//����������
                    {
                        if(!flag)//����
                        {
                            startSignal = 0;//������
                            //�ȴ洢��������
                            for(i=0; i<20; i++)
                            {
                                if(outputData[i]>dataMax)
                                {
                                    dataMax = outputData[i];
                                }
                                if(outputData[i]<dataMin)
                                {
                                    dataMin = outputData[i];
                                }
                            }
                            Vpp1 = (dataMax - dataMin)*20/65535.0 * FACTOR2;//���������ѹ ��λV

                            //���ô���
                            Relay_RL = 1;
                            flag = 1;
                            //��FPGA�������ʼ���
                            startSignal = 1;//�ٴο�ʼ���

                            dataMax=0;
                            dataMin=65535;
                        }
                        else//����
                        {
                            flag = 0;
                            startSignal = 0;//������
                            //���ݴ���
                            for(i=0; i<20; i++)
                            {
                                if(outputData[i]>dataMax)
                                {
                                    dataMax = outputData[i];
                                }
                                if(outputData[i]<dataMin)
                                {
                                    dataMin = outputData[i];
                                }
                            }
                            Vpp2 = (dataMax - dataMin)*20/65535.0 * FACTOR2;//���������ѹ ��λV
                            Ro = (Vpp1/Vpp2-1)*RL;//�õ��������
                            dataMax=0;
                            dataMin=65535;

                            WM_SendMessage(WM_GetClientWindow(hWin_user), &message);//������Ϣ������,��ʾ
                        }

                    }
                    if(mode==3)//�������
                    {
                        startSignal = 0;//������
                        //���ݴ���
                        for(i=0; i<20; i++)
                        {
                            if(outputData[i]>dataMax)
                            {
                                dataMax = outputData[i];
                            }
                            if(outputData[i]<dataMin)
                            {
                                dataMin = outputData[i];
                            }
                        }
                        Vpp1 = (dataMax - dataMin)*20/65535.0 * FACTOR2;//���������ѹ ��λV
                        Av=Vpp1*1000/20;//�õ����� ��λV/V
                        dataMax=0;
                        dataMin=65535;

                        WM_SendMessage(WM_GetClientWindow(hWin_user), &message);//������Ϣ������,��ʾ
                    }
                    end_flag = 0;
                }
            }
        }

        OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
    }
}

//��Ƶ���Բ�������
void amfre_task(void *p_arg)
{
	u8 i=0;
	u16 fact=1;
    u16 adcx;
    u8 db;
	u8 dbMax=0;
    OS_ERR err;
    WM_MESSAGE message;
    message.MsgId = WM_SHOW_MSG;

    while(1)
    {
        if(mode==4&&ADC_flag)
        {
            //ɨƵ
			printf("%d: %d\r\n",i,freq);
            Freq_convert(freq);
			i++;
            if(i<=90)
            {
                freq=(i+10)*fact;
				if(i==90) 
				{
					i=0;
					fact *= 10;
				}
            }
            if(freq>=1000000)
            {
                ADC_flag = 0;//��������
				freq=10;
				i=0;
				fact=1;
            }
            //ADC��ȡ��ѹ
            adcx=Get_Adc_Average(ADC_Channel_5,10);//��ȡͨ��5��ת��ֵ��10��ȡƽ��
            db=20*log10(adcx);          //תΪdb
            if(db>dbMax) dbMax = db;//�ҳ����db
			else
			{
				if(dbMax-3==db)//�ҵ�-3db����Ƶ��
				{
					freq_3dbH = freq;
					WM_SendMessage(WM_GetClientWindow(hWin_user), &message);//������Ϣ������,��ʾ
				}
			}
			if(freq==330)//330Hz
			{
				VL=(float)adcx*(3.3/4096);//�����ĵ�·���޽�ֹƵ�ʶ�Ӧ��ֵ
			}
			if(freq==150000)//150KHz
			{
				VH=(float)adcx*(3.3/4096);//�����ĵ�·���޽�ֹƵ�ʶ�Ӧ��ֵ
			}
            //��ͼ
            GRAPH_DATA_YT_AddValue(pdataGRP, db);//��ֵ������
			delay_ms(10);
        }
		
        OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
    }
}

//���ϼ������
void test_task(void *p_arg)
{
    OS_ERR err;
	u16 dataMax=0;
    u16 dataMin=65535;
	u8 i;
	double Vmax=0;
	double Vpp=0;
	double Vdc=0;
	u16 adcx;
	double Vm=0;
	
	WM_MESSAGE message;
    message.MsgId = WM_SHOW_MSG;

    while(1)
    {
		if(mode == 5)
		{			
			//�趨DDS����
			Freq_convert(1000);//1000Hz
			Write_Amplitude(20);//20mV
			
			test = 1;//�������ѹ
			//����� �������費�� ���� 
			Relay_inout = 1;
			Relay_RS = 1;
			Relay_RL = 0;
			startSignal = 1;//�������
			
			//��ʼ����
			while(test==1)//�������ѹ����
			{
				if(MySPI_ReadByte() == 0x11)//�ɹ����յ�������
				{
					if(end_flag == 0xff)//�������
					{
						startSignal = 0;//������
						//���ݴ���
						for(i=0; i<20; i++)
						{
							if(outputData[i]>dataMax)
							{
								dataMax = outputData[i];
							}
							if(outputData[i]<dataMin)
							{
								dataMin = outputData[i];
							}
						}
						Vmax = dataMax*20/65535.0 * FACTOR2;//��������ѹֵ ��λV
						Vpp = (dataMax - dataMin)*20/65535.0 * FACTOR2;//������ֵ ��λV
						Vdc = Vmax - Vpp/2;//����ֱ��ƫ��
						dataMax=0;
						dataMin=65535;
						//1.�ж�R4��·
						if(Vdc<0.1)//ֱ����ѹС��100mV
						{
							testResult = 4;//R4��·
							currentState = 0;
							break;
						}
						//2.�ж�R2��·
						if(fabs(Vpp-0.3)<0.1 && fabs(Vdc-4.3)<0.5)//���ֵ�ӽ�0.3V ֱ����4.3V����
						{
							testResult = 6;//R2��·
							currentState = 0;
							break;
						}
						//3.�жϴ�ֱ�����
						if(Vpp<0.03)//���ֵС��40mV	
						{
							//R1��·
							if(fabs(Vdc-10.62)<0.5)//ֱ����ѹΪ10.62V����
							{
								testResult = 1;//R1��·
								currentState = 0;
								break;
							}
							//R3��·
							if(fabs(Vdc-0.2)<0.05)//����ֱ�����Ϊ200mV����
							{
								testResult = 7;//R3��·
								currentState = 0;
								break;
							}
							if(fabs(Vdc-12)<0.5)//ֱ����ѹΪ12V���� R2��· R3��· R1��· R4��·
							{
								test = 2;//��һ������������� �� ���ص�ѹ
								break;
							}
							//C1��·
							if(fabs(Vdc-7.35)<0.5)//ֱ����ѹΪ7.35V����
							{
								testResult = 9;//C1��·
								currentState = 0;
								break;
							}
						}
						//4.�ж�C2��·
						if(fabs(Vdc-7.35)<0.5 && fabs(Vpp-0.078)<0.04)//�н������ Ϊ78mV����
						{
							testResult = 10;//C2��·
							currentState = 0;
							break;
						}
						//ִ�е����û�з������⣬��ȥ���Ƶ����
						test = 4;
					}
				}
			}
			
			while(test==2)//�����������̣�ֻ����ֱ�����12V�Ż����--R2��·��R1��·,�������������ص�ѹ��
			{
				//�̵������Ʋ�����������· ���� ������
				Relay_inout = 0;
				Relay_RS = 0;
				Relay_RL = 0;
				//��FPGA�������ʼ���
				startSignal = 1;
				if(MySPI_ReadByte() == 0x11)//�ɹ����յ�������
				{
					if(end_flag == 0xff)//�������
					{
						startSignal = 0;//������
						//���ݴ���
						for(i=0; i<20; i++)
						{
							if(outputData[i]>dataMax)
							{
								dataMax = outputData[i];
							}
							if(outputData[i]<dataMin)
							{
								dataMin = outputData[i];
							}
						}
						Vpp = (dataMax - dataMin)*20*1000/65535.0/FACTOR1;//�����ѹ ��λmV �������20V
                        Ri = (20-Vpp)*RS/Vpp;//�õ��������
						dataMax=0;
						dataMin=65535;
						//R2��·
						if(Ri<100)//�������С��100ohm
						{
							testResult = 2;//R2��·
							currentState = 0;
							break;
						}
						//R1��·
						if(abs(Ri-15000)<1000)//�������ӽ�R2=15K
						{
							testResult = 5;//R1��·
							currentState = 0;
							break;
						}
						test = 3;
					}
				}
			}
						
			while(test==3)//���Դ���������̣�R3��·�����ֱ��12V�����������������һ�����R4��·��
			{
				//�̵������Ʋ��������· �����  ���� 
				Relay_inout = 1;
				Relay_RS = 1;
				Relay_RL = 1;
				//��FPGA�������ʼ���
				startSignal = 1;
				if(MySPI_ReadByte() == 0x11)//�ɹ����յ�������
				{
					if(end_flag == 0xff)//�������
					{
						startSignal = 0;//������
						//���ݴ���
						for(i=0; i<20; i++)
						{
							if(outputData[i]>dataMax)
							{
								dataMax = outputData[i];
							}
							if(outputData[i]<dataMin)
							{
								dataMin = outputData[i];
							}
						}
						Vmax = dataMax*20/65535.0 * FACTOR2;//��������ѹֵ ��λV
						Vpp = (dataMax - dataMin)*20/65535.0 * FACTOR2;//������ֵ ��λV
						Vdc = Vmax - Vpp/2;//����ֱ��ƫ��
						dataMax=0;
						dataMin=65535;
						//R3��·
						if(fabs(Vdc-12)<0.5)//����ֱ�����12V
						{
							testResult = 3;//R3��·
							currentState = 0;
							break;
						}
						//R4��·
						else
						{
							testResult = 8;//R4��·
							currentState = 0;
							break;
						}
					}
				}
			}
			while(test==4)//���Է�Ƶ���Թ���
			{
				//�̵������Ʋ��������· �����  ���� 
				Relay_inout = 1;
				Relay_RS = 1;
				Relay_RL = 0;
				Freq_convert(150000);//�������޽�ֹƵ��150kHz
				delay_ms(10);//��ʱ
				//ADC��ȡ��ѹ
				adcx=Get_Adc_Average(ADC_Channel_5,10);//��ȡͨ��5��ת��ֵ��10��ȡƽ��
				Vm=(float)adcx*(3.3/4096); //���޽�ֹƵ��150kHz�µ���Чֵ
				//C3��·
				if(Vm>VH && Vm-VH>0.1)//���޽�ֹƵ�ʱ�� 
				{
					testResult = 11;//C3��·
					currentState = 0;
					break;
				}
				//C3����
				if(VH>Vm && VH-Vm>0.08)//���޽�ֹƵ�ʱ�С
				{
					testResult = 14;//C3����
					currentState = 0;
					break;
				}
				Freq_convert(330);//�������޽�ֹƵ��330Hz
				delay_ms(10);//��ʱ
				//ADC��ȡ��ѹ
				adcx=Get_Adc_Average(ADC_Channel_5,10);//��ȡͨ��5��ת��ֵ��10��ȡƽ��
				Vm=(float)adcx*(3.3/4096); //���޽�ֹƵ��150kHz�µ���Чֵ
				//C2����
				if(Vm>VL && Vm-VL>0.08)//���޽�ֹƵ�ʱ�С
				{
					testResult = 13;//C2����
					currentState = 0;
					break;
				}
				//���Խ��� ���ж�����
				currentState = 1;
			}
		}

		WM_SendMessage(WM_GetClientWindow(hWin_user), &message);//������Ϣ������,��ʾ
		
        OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//��ʱ100ms
    }
}



