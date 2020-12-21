#include "led.h"
#include "sys.h"
#include "key.h"
#include "exti.h"
#include "usart.h"
#include "lcd.h"
#include "adc.h"
#include "stm32f10x_adc.h"

u16   buf[350];
u16   table[11]  ={0,25,50,75,100,125,150,175,200,225,250};
u16		table1[41] ={0,5,10,15,20,30,35,40,45,55,60,65,70,80,85,
										90,95,105,110,115,120,130,135,140,145,155,
										160,165,170,180,185,190,195,205,210,215,220,
										230,235,240,245};
u16 Ypos1=100,Ypos2=100,Xpos1,Xpos2;
u16 adcx,dsl;
float temp1;
u16  t=0;
u16  h=0;
u16 d=0;
u32  tim=1;
u32  bei=1;
u16 Yinit=75;
u8 a;
u8 len;										

void drawgaid() //画网格
{
	u16 x,y;
	for(x=0;x<251;x=x+25)
		for(y=0;y<201;y=y+5)
		{
			LCD_Fast_DrawPoint(x,y,YELLOW );
		}
	for(y=0;y<201;y=y+25)
		for(x=0;x<251;x=x+5)
		{
			LCD_Fast_DrawPoint(x,y,YELLOW );
		}
}


void clear_point(u16 hang)//更新显示屏当前列
{
	u8 index_clear_lie = 0; 
	POINT_COLOR = BLACK ;
	for(index_clear_lie = 0;index_clear_lie <201;index_clear_lie++)
	{		
		LCD_DrawPoint(hang,index_clear_lie );
	}
	if(hang==table[h])//判断hang是否为25的倍数
	{
		for(index_clear_lie = 0;index_clear_lie <201;index_clear_lie=index_clear_lie+5)
		{		
			LCD_Fast_DrawPoint(hang ,index_clear_lie,YELLOW);
		}
		h++;
		if(h>10) h=0;
	}
	if(hang ==table1[d])//判断hang是否为5的倍数
	{
		for(index_clear_lie = 0;index_clear_lie <201;index_clear_lie=index_clear_lie+25)
		{		
			LCD_Fast_DrawPoint(hang ,index_clear_lie,YELLOW);
		}
		d++;
		if(d>40) d=0;
	}
	POINT_COLOR=RED;	
}

void DrawOscillogram()//画波形图
{
		for(t=0;t<251;t++)//存储AD数值
		{
			buf[t] =Get_Adc(ADC_Channel_1);
			if(tim>1)
				delay_us (tim );//改变AD取样间隔
		}
		for(t=0;t<251;t++)
		{
			clear_point(t );	
			Ypos2=Yinit+buf[t]*100/4096;//转换坐标
			Ypos2=Ypos2*bei;
			if(Ypos2 >200) Ypos2 =200; //超出范围不显示
			LCD_DrawLine (t ,Ypos1 , t+1 ,Ypos2  );
			Ypos1 =Ypos2 ;
		}	
}
float get_vpp(void)	   //获取峰峰值
{
	
	u32 max_data=buf[0];
	u32 min_data=buf[0];
	u32 n=0;
	float Vpp=0;
	for(n = 1;n<201;n++)
	{
		if(buf[n]>max_data)
		{
			max_data = buf[n];
		}
		if(buf[n]<min_data)
		{
			min_data = buf[n];
		}			
	} 	
	Vpp = (float)(max_data-min_data);
	Vpp = Vpp*(3300.0/4095);
	return Vpp;	
}
void Usartkeyscan()  //蓝牙按键扫描
{
		if(USART_RX_STA&0x8000)	 
		 {
				 len =USART_RX_STA &0x3fff;
				 for(t=0;t<len;t++)
				 {
					 a=USART_RX_BUF[t];
					 USART1->DR =a;
					 while((USART1->SR&0X40)==0);//等待发送结束
				 }
				 USART_RX_STA=0;
				 if( USART_RX_BUF[0]=='K') Yinit=Yinit -25 ;
				 if( USART_RX_BUF[0]=='O') Yinit =Yinit +25 ;
				 if( USART_RX_BUF[0]=='J') 
				 {
					 tim /=2 ;
					 LCD_ShowxNum(284,220,tim ,3,16,0);
				 }
				 if( USART_RX_BUF[0]=='D') 
				 {
					 tim *=2 ;
					 LCD_ShowxNum(284,220,tim ,3,16,0);
				 }
				 if( USART_RX_BUF[0]=='S') 
				 { 
						bei =bei*2;
					 if(bei>3) bei=1;
				 }
				 
				 if(USART_RX_BUF[0]=='O'&&USART_RX_BUF[1]=='K')
				 {
					 while(USART_RX_STA==0&& d<20)
					 {
							LED0 =!LED0 ;
							d++;
							delay_ms (1000);
					 }
					 d=0;
				 }
			 
		 }
		 
}


int main()
 {

	 NVIC_Configuration();// 设置中断优先级分组
	 delay_init();
	 LED_Init ();
	 KEY_Init ();
	 EXTIX_Init();
	 uart_init(9600);
	 LCD_Init();
	 Adc_Init();		  		//ADC初始化
	 drawgaid();
	 POINT_COLOR=RED; 
	 LCD_ShowString(0,210,200,24,24,"Vpp=0000mv");
	 LCD_ShowxNum(284,220,tim ,3,16,0);
	 

	 while (1)
	 {	
			 Usartkeyscan ();//蓝牙按键扫描
			 DrawOscillogram();//画波形
			 temp1=get_vpp();//峰峰值mv		
			 LCD_ShowxNum(49,210,temp1,4,24,0);	//显示峰峰值mv
			 LED0=!LED0;//判断程序是否运行
	
	 }
 } 