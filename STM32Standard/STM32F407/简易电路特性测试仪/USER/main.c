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

#define RS 15000//采样电阻RS 单位欧姆
#define RL 1000//负载RL 单位欧姆
#define FACTOR1 334.33//INA增益=1+50K/150=334.33
#define FACTOR2 1.7259//实测电压衰减为0.5794倍 1.7259=1/0.5794

extern WM_HWIN hWin_user;//对话框
extern GRAPH_DATA_Handle  pdataGRP;//绘图数据

extern u8 end_flag;
extern u16 outputData[20];//存储检测到的输出数据

extern u8 mode;
extern u8 ADC_flag;

u32 freq = 10;

u16 Ri;
u16 Ro;
double Av;
double VH;//正常的电路上限截止频率对应的值
double VL;//正常的电路下限截止频率对应的值
u32 freq_3dbH;//上限截止pl
u32 freq_3dbL;//下限截止pl

//1代表没出故障，0代表出故障
u8 currentState;
//1:测空载输出
//2：测输入电阻
//3：测带载输出
//4：测幅频
u8 test;
//1-4 R1-R4短路
//5-8 R1-R4开路
//9-11 C1-C3开路
//12-14 C1-C3两倍
u8 testResult;

//任务优先级
#define START_TASK_PRIO				3
//任务堆栈大小
#define START_STK_SIZE 				1024
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//TOUCH任务
//设置任务优先级
#define TOUCH_TASK_PRIO				4
//任务堆栈大小
#define TOUCH_STK_SIZE				128
//任务控制块
OS_TCB TouchTaskTCB;
//任务堆栈
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch任务
void touch_task(void *p_arg);

//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO 				5
//任务堆栈大小
#define LED0_STK_SIZE				128
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0任务
void led0_task(void *p_arg);

//EMWINDEMO任务
//设置任务优先级
#define EMWINDEMO_TASK_PRIO			6
//任务堆栈大小
#define EMWINDEMO_STK_SIZE			2048
//任务控制块
OS_TCB EmwindemoTaskTCB;
//任务堆栈
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task任务
void emwindemo_task(void *p_arg);

//接收数据任务
//设置任务优先级
#define RECEIVE_TASK_PRIO 			7
//任务堆栈大小
#define RECEIVE_STK_SIZE			128
//任务控制块
OS_TCB ReceiveTaskTCB;
//任务堆栈
CPU_STK RECEIVE_TASK_STK[RECEIVE_STK_SIZE];
//receive任务
void receive_task(void *p_arg);

//幅频特性测量任务
//设置任务优先级
#define AMFRE_TASK_PRIO 			8
//任务堆栈大小
#define AMFRE_STK_SIZE				128
//任务控制块
OS_TCB AmfreTaskTCB;
//任务堆栈
CPU_STK AMFRE_TASK_STK[AMFRE_STK_SIZE];
//amfre任务
void amfre_task(void *p_arg);

//故障检测任务
//设置任务优先级
#define TEST_TASK_PRIO 				9
//任务堆栈大小
#define TEST_STK_SIZE				128
//任务控制块
OS_TCB TestTaskTCB;
//任务堆栈
CPU_STK TEST_TASK_STK[TEST_STK_SIZE];
//test任务
void test_task(void *p_arg);

int main(void)
{
    OS_ERR err;
    CPU_SR_ALLOC();

    delay_init(168);       	//延时初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//中断分组配置
    uart_init(115200);    	//串口波特率设置
    TFTLCD_Init();			//初始化LCD
	W25QXX_Init();			//初始化W25Q128

    LED_Init();   			//LED初始化
    MySPI_Init();			//SPI传输初始化
    AD9910_Init();			//AD9910初始化
    Adc_Init();				//片内ADC初始化
    myGPIO_init();			//自定义的GPIO

    //初始状态：1kHz，Vpp：20mV
    Freq_convert(1000); //写频率
    Write_Amplitude(20); //写幅度

    FSMC_SRAM_Init(); 		//SRAM初始化
    mem_init(SRAMIN); 		//内部RAM初始化
    mem_init(SRAMEX); 		//外部RAM初始化
    mem_init(SRAMCCM);		//CCM初始化

    TP_Init();				//初始化触摸屏
    OSInit(&err);			//初始化UCOSIII
    OS_CRITICAL_ENTER();	//进入临界区
    //创建开始任务
    OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
                 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
    OS_CRITICAL_EXIT();	//退出临界区
    OSStart(&err);  //开启UCOSIII
    while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    p_arg = p_arg;

    CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);  	//统计任务
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
    //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
    OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);
#endif
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//开启CRC时钟
    WM_SetCreateFlags(WM_CF_MEMDEV); //启动所有窗口的存储设备
    GUI_Init();  			//STemWin初始化

    OS_CRITICAL_ENTER();	//进入临界区
    //STemWin Demo任务
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
    //触摸屏任务
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
    //LED0任务
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
    //接收数据任务
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
    //幅频特性测量任务
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
    //故障检测任务
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
    OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务
    OS_CRITICAL_EXIT();	//退出临界区
}

//EMWINDEMO任务
void emwindemo_task(void *p_arg)
{
    //更换皮肤
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

//TOUCH任务
void touch_task(void *p_arg)
{
    OS_ERR err;
    while(1)
    {
        GUI_TOUCH_Exec();
        OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
    }
}

//LED0任务
void led0_task(void *p_arg)
{
    OS_ERR err;
    while(1)
    {
        LED0 = !LED0;
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//延时500ms
    }
}

//接收数据任务
void receive_task(void *p_arg)
{
    u8 flag = 0;//空载
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
                    //数据处理
                    if(mode==1)//检测输入电阻
                    {
                        startSignal = 0;//检测完毕
                        //数据处理
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
                        Vpp1 = (dataMax - dataMin)*20*1000/65535.0/FACTOR1;//输入电压 单位mV 最大量程20V
                        Ri = (20-Vpp1)*RS/Vpp1;//得到输入电阻
                        dataMax=0;
                        dataMin=65535;

                        WM_SendMessage(WM_GetClientWindow(hWin_user), &message);//发送消息给窗口,显示
                    }
                    if(mode==2)//检测输出电阻
                    {
                        if(!flag)//空载
                        {
                            startSignal = 0;//检测完毕
                            //先存储空载数据
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
                            Vpp1 = (dataMax - dataMin)*20/65535.0 * FACTOR2;//空载输出电压 单位V

                            //设置带载
                            Relay_RL = 1;
                            flag = 1;
                            //给FPGA发送命令开始检测
                            startSignal = 1;//再次开始检测

                            dataMax=0;
                            dataMin=65535;
                        }
                        else//带载
                        {
                            flag = 0;
                            startSignal = 0;//检测完毕
                            //数据处理
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
                            Vpp2 = (dataMax - dataMin)*20/65535.0 * FACTOR2;//带载输出电压 单位V
                            Ro = (Vpp1/Vpp2-1)*RL;//得到输出电阻
                            dataMax=0;
                            dataMin=65535;

                            WM_SendMessage(WM_GetClientWindow(hWin_user), &message);//发送消息给窗口,显示
                        }

                    }
                    if(mode==3)//检测增益
                    {
                        startSignal = 0;//检测完毕
                        //数据处理
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
                        Vpp1 = (dataMax - dataMin)*20/65535.0 * FACTOR2;//空载输出电压 单位V
                        Av=Vpp1*1000/20;//得到增益 单位V/V
                        dataMax=0;
                        dataMin=65535;

                        WM_SendMessage(WM_GetClientWindow(hWin_user), &message);//发送消息给窗口,显示
                    }
                    end_flag = 0;
                }
            }
        }

        OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
    }
}

//幅频特性测量任务
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
            //扫频
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
                ADC_flag = 0;//结束测量
				freq=10;
				i=0;
				fact=1;
            }
            //ADC读取电压
            adcx=Get_Adc_Average(ADC_Channel_5,10);//获取通道5的转换值，10次取平均
            db=20*log10(adcx);          //转为db
            if(db>dbMax) dbMax = db;//找出最大db
			else
			{
				if(dbMax-3==db)//找到-3db上限频率
				{
					freq_3dbH = freq;
					WM_SendMessage(WM_GetClientWindow(hWin_user), &message);//发送消息给窗口,显示
				}
			}
			if(freq==330)//330Hz
			{
				VL=(float)adcx*(3.3/4096);//正常的电路下限截止频率对应的值
			}
			if(freq==150000)//150KHz
			{
				VH=(float)adcx*(3.3/4096);//正常的电路上限截止频率对应的值
			}
            //绘图
            GRAPH_DATA_YT_AddValue(pdataGRP, db);//赋值到曲线
			delay_ms(10);
        }
		
        OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
    }
}

//故障检测任务
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
			//设定DDS参数
			Freq_convert(1000);//1000Hz
			Write_Amplitude(20);//20mV
			
			test = 1;//测输出电压
			//测输出 采样电阻不接 空载 
			Relay_inout = 1;
			Relay_RS = 1;
			Relay_RL = 0;
			startSignal = 1;//开启检测
			
			//开始测试
			while(test==1)//测输出电压过程
			{
				if(MySPI_ReadByte() == 0x11)//成功接收到数据了
				{
					if(end_flag == 0xff)//检查数据
					{
						startSignal = 0;//检测完毕
						//数据处理
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
						Vmax = dataMax*20/65535.0 * FACTOR2;//计算最大电压值 单位V
						Vpp = (dataMax - dataMin)*20/65535.0 * FACTOR2;//计算峰峰值 单位V
						Vdc = Vmax - Vpp/2;//计算直流偏置
						dataMax=0;
						dataMin=65535;
						//1.判断R4短路
						if(Vdc<0.1)//直流电压小于100mV
						{
							testResult = 4;//R4短路
							currentState = 0;
							break;
						}
						//2.判断R2开路
						if(fabs(Vpp-0.3)<0.1 && fabs(Vdc-4.3)<0.5)//峰峰值接近0.3V 直流在4.3V左右
						{
							testResult = 6;//R2开路
							currentState = 0;
							break;
						}
						//3.判断纯直流输出
						if(Vpp<0.03)//峰峰值小于40mV	
						{
							//R1短路
							if(fabs(Vdc-10.62)<0.5)//直流电压为10.62V左右
							{
								testResult = 1;//R1短路
								currentState = 0;
								break;
							}
							//R3开路
							if(fabs(Vdc-0.2)<0.05)//空载直流输出为200mV左右
							{
								testResult = 7;//R3开路
								currentState = 0;
								break;
							}
							if(fabs(Vdc-12)<0.5)//直流电压为12V左右 R2短路 R3短路 R1开路 R4开路
							{
								test = 2;//进一步测试输入电阻 和 带载电压
								break;
							}
							//C1开路
							if(fabs(Vdc-7.35)<0.5)//直流电压为7.35V左右
							{
								testResult = 9;//C1开路
								currentState = 0;
								break;
							}
						}
						//4.判断C2开路
						if(fabs(Vdc-7.35)<0.5 && fabs(Vpp-0.078)<0.04)//有交流输出 为78mV左右
						{
							testResult = 10;//C2开路
							currentState = 0;
							break;
						}
						//执行到如果没有发现问题，则去测幅频特性
						test = 4;
					}
				}
			}
			
			while(test==2)//测输入电阻过程（只有在直流测得12V才会进入--R2短路，R1开路,若都不是则测带载电压）
			{
				//继电器控制采样电阻接入电路 空载 测输入
				Relay_inout = 0;
				Relay_RS = 0;
				Relay_RL = 0;
				//给FPGA发送命令开始检测
				startSignal = 1;
				if(MySPI_ReadByte() == 0x11)//成功接收到数据了
				{
					if(end_flag == 0xff)//检查数据
					{
						startSignal = 0;//检测完毕
						//数据处理
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
						Vpp = (dataMax - dataMin)*20*1000/65535.0/FACTOR1;//输入电压 单位mV 最大量程20V
                        Ri = (20-Vpp)*RS/Vpp;//得到输入电阻
						dataMax=0;
						dataMin=65535;
						//R2短路
						if(Ri<100)//输入电阻小于100ohm
						{
							testResult = 2;//R2短路
							currentState = 0;
							break;
						}
						//R1开路
						if(abs(Ri-15000)<1000)//输入电阻接近R2=15K
						{
							testResult = 5;//R1开路
							currentState = 0;
							break;
						}
						test = 3;
					}
				}
			}
						
			while(test==3)//测试带载输出过程（R3短路（输出直流12V），若不是则是最后一种情况R4开路）
			{
				//继电器控制采样电阻短路 测输出  带载 
				Relay_inout = 1;
				Relay_RS = 1;
				Relay_RL = 1;
				//给FPGA发送命令开始检测
				startSignal = 1;
				if(MySPI_ReadByte() == 0x11)//成功接收到数据了
				{
					if(end_flag == 0xff)//检查数据
					{
						startSignal = 0;//检测完毕
						//数据处理
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
						Vmax = dataMax*20/65535.0 * FACTOR2;//计算最大电压值 单位V
						Vpp = (dataMax - dataMin)*20/65535.0 * FACTOR2;//计算峰峰值 单位V
						Vdc = Vmax - Vpp/2;//计算直流偏置
						dataMax=0;
						dataMin=65535;
						//R3短路
						if(fabs(Vdc-12)<0.5)//带载直流输出12V
						{
							testResult = 3;//R3短路
							currentState = 0;
							break;
						}
						//R4开路
						else
						{
							testResult = 8;//R4开路
							currentState = 0;
							break;
						}
					}
				}
			}
			while(test==4)//测试幅频特性过程
			{
				//继电器控制采样电阻短路 测输出  空载 
				Relay_inout = 1;
				Relay_RS = 1;
				Relay_RL = 0;
				Freq_convert(150000);//测试上限截止频率150kHz
				delay_ms(10);//延时
				//ADC读取电压
				adcx=Get_Adc_Average(ADC_Channel_5,10);//获取通道5的转换值，10次取平均
				Vm=(float)adcx*(3.3/4096); //上限截止频率150kHz下的有效值
				//C3开路
				if(Vm>VH && Vm-VH>0.1)//上限截止频率变大 
				{
					testResult = 11;//C3开路
					currentState = 0;
					break;
				}
				//C3两倍
				if(VH>Vm && VH-Vm>0.08)//上限截止频率变小
				{
					testResult = 14;//C3两倍
					currentState = 0;
					break;
				}
				Freq_convert(330);//测试下限截止频率330Hz
				delay_ms(10);//延时
				//ADC读取电压
				adcx=Get_Adc_Average(ADC_Channel_5,10);//获取通道5的转换值，10次取平均
				Vm=(float)adcx*(3.3/4096); //上限截止频率150kHz下的有效值
				//C2两倍
				if(Vm>VL && Vm-VL>0.08)//下限截止频率变小
				{
					testResult = 13;//C2两倍
					currentState = 0;
					break;
				}
				//测试结束 则判断正常
				currentState = 1;
			}
		}

		WM_SendMessage(WM_GetClientWindow(hWin_user), &message);//发送消息给窗口,显示
		
        OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//延时100ms
    }
}



