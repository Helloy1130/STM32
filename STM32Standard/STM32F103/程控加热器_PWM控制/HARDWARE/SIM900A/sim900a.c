#include "sim900a.h"
#include "usart.h"		
#include "delay.h"		 	 	 	 
#include "lcd.h" 	 
#include "touch.h" 
#include "ds18b20.h"
#include "beep.h"
#include "pid.h"

u8 temp[4];
u8 set_flag;			//设定温度的信号
short temperature;//温度传感器接收到的值
double real_temp;
extern double set_temp;

double Ek;
double Ek1;
double Ek2;

double Kp = 7.5;
double Ki = 0;
double Kd = 0;


double pwm_pulse1 = 0;//低电平所占宽度	PWM1输出 占空比越大，加热越快
u16 pwm_pulse2;
double P = 0;
	

void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)//显示汉字
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
	{   		    
	    for(i=0;i<csize;i++)*mat++=0x00;//填充满格
	    return; //结束访问
	}          
	if(ql<0x7f)ql-=0x40;//注意!
	else ql-=0x41;
	qh-=0x81;     												    
} 

void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[72];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	if(size!=12&&size!=16&&size!=24)return;	//不支持的size
	Get_HzMat(font,dzk,size);	//得到相应大小的点阵数据 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR); 
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}  
}
		
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)//显示字串
{					
	u16 x0=x;
	u16 y0=y;							  	  
    u8 bHz=0;     //字符或者中文  	    				    				  	  
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//中文 
	        else              //字符
	        {      
                if(x>(x0+width-size/2))//换行
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//越界返回      
		        if(*str==13)//换行符号
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,mode);//有效部分写入 
				str++; 
		        x+=size/2; //字符,为全字的一半 
	        }
        }else//中文 
        {     
            bHz=0;//有汉字库    
            if(x>(x0+width-size))//换行
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//越界返回  						     
	        Show_Font(x,y,str,size,mode); //显示这个汉字,空心显示 
	        str+=2; 
	        x+=size;//下一个汉字偏移	    
        }						 
    }   
}  			 		 
//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用Show_Str显示
//len:指定要显示的宽度			  
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len)
{
	u16 strlenth=0;
   	strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)Show_Str(x,y,lcddev.width,lcddev.height,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	    Show_Str(strlenth+x,y,lcddev.width,lcddev.height,str,size,1);
	}
}     

u16 time=0;

//键盘码表
const u8* kbd_tbl1[15]={"1","2","3","4","5","6","7","8","9","","0",".","DEL","SET",""};
u8** kbd_tbl;
u8* kbd_fn_tbl[2];
//加载键盘界面（尺寸为240*140）
//x,y:界面起始坐标（320*240分辨率的时候，x必须为0）
void sim900a_load_keyboard(u16 x,u16 y,u8 **kbtbl)
{
	u16 i;
	POINT_COLOR=RED;
	kbd_tbl=kbtbl;
	LCD_Fill(x,y,x+240,y+140,WHITE);
	LCD_DrawRectangle(x,y,x+240,y+140);						   
	LCD_DrawRectangle(x+80,y,x+160,y+140);	 
	LCD_DrawRectangle(x,y+28,x+240,y+56);
	LCD_DrawRectangle(x,y+84,x+240,y+112);
	POINT_COLOR=BLUE;
	for(i=0;i<16;i++)
	{
	if(i<15)Show_Str_Mid(x+(i%3)*80,y+6+28*(i/3),(u8*)kbd_tbl[i],16,80);
		else Show_Str_Mid(x+(i%3)*80,y+6+28*(i/3),kbd_fn_tbl[i-13],16,80); 
	}  		 					   
}

//按键状态设置
//x,y:键盘坐标
//key:键值（0~8）
//sta:状态，0，松开；1，按下；
void sim900a_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	//u16 i=keyx/3,j=keyx%3;time=time+1;

	if(keyx>15)return;	 		 
}

//得到触摸屏的输入
//x,y:键盘坐标
//返回值：按键键值（1~15有效；0,无效）
u8 sim900a_get_keynum(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x=0;//0,没有任何按键按下；1~15，1~15号按键按下
	u8 key=0;
	tp_dev.scan(0); 		 
	if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
	{	
		for(i=0;i<5;i++)
		{
			for(j=0;j<3;j++)
			{
			 	if(tp_dev.x[0]<(x+j*80+80)&&tp_dev.x[0]>(x+j*80)&&tp_dev.y[0]<(y+i*28+28)&&tp_dev.y[0]>(y+i*28))
				{		
					key=i*3+j+1;
				
					break;	 		   
				}
			}					
			if(key)
			{	   
				if(key_x==key)key=0;
				else 
				{
					sim900a_key_staset(x,y,key_x-1,0);
					key_x=key;
					sim900a_key_staset(x,y,key_x-1,1);
				}
				break;
			}
		}  
	}	
	else if(key_x) 
	{
		sim900a_key_staset(x,y,key_x-1,0);
		key_x=0;
	} 
	return key; 
}

//SIM900A发短信测试 
void sim900a_sms_send_test(void)
{   	
	u8 numlen=0;		 
	u8 numbuf[20]; 		
	u8 key=0;
	u8 t=0;
	u8 flag = 1;

	BEEP_Init();
	uart_init(9600);	 	//串口初始化为9600
	LCD_Clear(WHITE);
	sim900a_load_keyboard(0,180,(u8**)kbd_tbl1);//显示键盘
	POINT_COLOR=BLUE;//设置字体为蓝色 
 	LCD_ShowString(60,20,200,16,16,"Temp:   . C");	
	LCD_ShowString(60,36,200,16,16,"Set :   . C");
	//LCD_ShowString(60,100,200,16,16,"P   :   . W");
			
	while(1)
	{	     		
 		if(t%10==0)//每100ms读取一次 显示温度值
		{									  
			temperature=DS18B20_Get_Temp();	
			if(temperature<0)
			{
				LCD_ShowChar(60+40,20,'-',16,0);			//显示负号
				temperature=-temperature;					//转为正数
			}else LCD_ShowChar(60+40,20,' ',16,0);			//去掉负号
			LCD_ShowNum(60+40+8,20,temperature/10,2,16);	//显示正数部分	    
   			LCD_ShowNum(60+40+32,20,temperature%10,1,16);	//显示小数部分 		   				   
			real_temp = temperature/10.0;
			
			
			LCD_ShowNum(60+40+8,36,(u16)(set_temp)%100,2,16);	//显示正数部分	    
   			LCD_ShowNum(60+40+32,36,(u16)(set_temp*10)%10,1,16);	//显示小数部分 
			
			/**********PID*********/
			//温度偏差
			Ek = set_temp - real_temp;
			//控制
			//if(Ek>3)//局部积分
			{
				if(pwm_pulse1+(Kp*(Ek - Ek1) + Ki*Ek + Kd*(Ek-2*Ek1+Ek2))>0)
					pwm_pulse1 = Kp*(Ek - Ek1) + Ki*Ek + Kd*(Ek-2*Ek1+Ek2) + pwm_pulse1;
				else	pwm_pulse1 = 0;
			}
//			else
//			{
//				if(flag==1) 
//				{
//					pwm_pulse1 = 0;
//					flag = 0;
//				}
//				if(pwm_pulse1+(Kp*(Ek - Ek1) + 0*Ek + Kd*(Ek-2*Ek1+Ek2))>0)
//					pwm_pulse1 = Kp*(Ek - Ek1) + 0*Ek + Kd*(Ek-2*Ek1+Ek2) + pwm_pulse1;
//				else	pwm_pulse1 = 0;
//			}
			
//			pid.set = set_temp;
//			pid.curr = real_temp;
//			pid_calc();
			
			if(Ek<-0.5&&set_temp!=0)
			{
				pwm_pulse2 = 50;
			}
			else
			{
				pwm_pulse2 = 100;
			}
			//printf("%.2lf\r\n", pwm_pulse1);
			TIM_SetCompare1(TIM1, (u16)pwm_pulse1);
			TIM_SetCompare4(TIM1, pwm_pulse2);
			Ek2 = Ek1;
			Ek1 = Ek;
		}
		
		if(t%100==0)
		{
			printf("%.2lf\r\n", real_temp);
		}
		t++;
		
		//温度超过限制时处理 
		if(temperature>=700)	//超出70
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_2);//声光报警
			TIM_SetCompare1(TIM1, 0);
			TIM_SetCompare4(TIM1, 50);
		}
		if(temperature<=300)//低于30
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_2);
			TIM_SetCompare1(TIM1, 500);
			TIM_SetCompare4(TIM1, 100);
		}
		else
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_2);//声光报警
		}
		
		key = sim900a_get_keynum(0,180);
		
		if(key)
		{  
			if(key<10||key==11||key==12)
			{
				if(numlen<15)
				{ 
					numbuf[numlen]=kbd_tbl[key-1][0];
					if(key==11)
					{
						temp[numlen] = 0;
					}
					if(key<10)
					{
						temp[numlen] = key;
					}
					if(key==12)
					{
						temp[numlen] = 10;
					}
				}
				numlen++;
			}
			else
			{
				if(key==13)
				{
					if(numlen) 
					{		
						numlen--;
					}
					LCD_Fill(30+54,50,256,80,WHITE);//删除 
					temp[numlen] = 99;
				}  
				if(key==14&&numlen)			
				{  
					set_flag = 1;
				}
				if(set_flag&&numlen>4)
				{
					LCD_ShowString(30+54,80,156,24,16,"DATA ERROR!");  
					set_flag = 0;
					return ;
				}
			} 
			numbuf[numlen]=0; 
			LCD_Fill(30+54,50,156,24,WHITE);
			Show_Str(30+54,50,156,24,numbuf,24,0); 	
		}
		
		if(set_flag == 1)
		{
			return;
		}
		delay_ms(10);

	} 
}
