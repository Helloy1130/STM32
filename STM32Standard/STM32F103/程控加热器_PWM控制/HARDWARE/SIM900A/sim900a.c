#include "sim900a.h"
#include "usart.h"		
#include "delay.h"		 	 	 	 
#include "lcd.h" 	 
#include "touch.h" 
#include "ds18b20.h"
#include "beep.h"
#include "pid.h"

u8 temp[4];
u8 set_flag;			//�趨�¶ȵ��ź�
short temperature;//�¶ȴ��������յ���ֵ
double real_temp;
extern double set_temp;

double Ek;
double Ek1;
double Ek2;

double Kp = 7.5;
double Ki = 0;
double Kd = 0;


double pwm_pulse1 = 0;//�͵�ƽ��ռ���	PWM1��� ռ�ձ�Խ�󣬼���Խ��
u16 pwm_pulse2;
double P = 0;
	

void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)//��ʾ����
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//�� ���ú���
	{   		    
	    for(i=0;i<csize;i++)*mat++=0x00;//�������
	    return; //��������
	}          
	if(ql<0x7f)ql-=0x40;//ע��!
	else ql-=0x41;
	qh-=0x81;     												    
} 

void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[72];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	if(size!=12&&size!=16&&size!=24)return;	//��֧�ֵ�size
	Get_HzMat(font,dzk,size);	//�õ���Ӧ��С�ĵ������� 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//�õ���������                          
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
		
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)//��ʾ�ִ�
{					
	u16 x0=x;
	u16 y0=y;							  	  
    u8 bHz=0;     //�ַ���������  	    				    				  	  
    while(*str!=0)//����δ����
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//���� 
	        else              //�ַ�
	        {      
                if(x>(x0+width-size/2))//����
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//Խ�緵��      
		        if(*str==13)//���з���
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,mode);//��Ч����д�� 
				str++; 
		        x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
	        }
        }else//���� 
        {     
            bHz=0;//�к��ֿ�    
            if(x>(x0+width-size))//����
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//Խ�緵��  						     
	        Show_Font(x,y,str,size,mode); //��ʾ�������,������ʾ 
	        str+=2; 
	        x+=size;//��һ������ƫ��	    
        }						 
    }   
}  			 		 
//��ָ����ȵ��м���ʾ�ַ���
//����ַ����ȳ�����len,����Show_Str��ʾ
//len:ָ��Ҫ��ʾ�Ŀ��			  
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

//�������
const u8* kbd_tbl1[15]={"1","2","3","4","5","6","7","8","9","","0",".","DEL","SET",""};
u8** kbd_tbl;
u8* kbd_fn_tbl[2];
//���ؼ��̽��棨�ߴ�Ϊ240*140��
//x,y:������ʼ���꣨320*240�ֱ��ʵ�ʱ��x����Ϊ0��
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

//����״̬����
//x,y:��������
//key:��ֵ��0~8��
//sta:״̬��0���ɿ���1�����£�
void sim900a_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	//u16 i=keyx/3,j=keyx%3;time=time+1;

	if(keyx>15)return;	 		 
}

//�õ�������������
//x,y:��������
//����ֵ��������ֵ��1~15��Ч��0,��Ч��
u8 sim900a_get_keynum(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x=0;//0,û���κΰ������£�1~15��1~15�Ű�������
	u8 key=0;
	tp_dev.scan(0); 		 
	if(tp_dev.sta&TP_PRES_DOWN)			//������������
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

//SIM900A�����Ų��� 
void sim900a_sms_send_test(void)
{   	
	u8 numlen=0;		 
	u8 numbuf[20]; 		
	u8 key=0;
	u8 t=0;
	u8 flag = 1;

	BEEP_Init();
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	LCD_Clear(WHITE);
	sim900a_load_keyboard(0,180,(u8**)kbd_tbl1);//��ʾ����
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
 	LCD_ShowString(60,20,200,16,16,"Temp:   . C");	
	LCD_ShowString(60,36,200,16,16,"Set :   . C");
	//LCD_ShowString(60,100,200,16,16,"P   :   . W");
			
	while(1)
	{	     		
 		if(t%10==0)//ÿ100ms��ȡһ�� ��ʾ�¶�ֵ
		{									  
			temperature=DS18B20_Get_Temp();	
			if(temperature<0)
			{
				LCD_ShowChar(60+40,20,'-',16,0);			//��ʾ����
				temperature=-temperature;					//תΪ����
			}else LCD_ShowChar(60+40,20,' ',16,0);			//ȥ������
			LCD_ShowNum(60+40+8,20,temperature/10,2,16);	//��ʾ��������	    
   			LCD_ShowNum(60+40+32,20,temperature%10,1,16);	//��ʾС������ 		   				   
			real_temp = temperature/10.0;
			
			
			LCD_ShowNum(60+40+8,36,(u16)(set_temp)%100,2,16);	//��ʾ��������	    
   			LCD_ShowNum(60+40+32,36,(u16)(set_temp*10)%10,1,16);	//��ʾС������ 
			
			/**********PID*********/
			//�¶�ƫ��
			Ek = set_temp - real_temp;
			//����
			//if(Ek>3)//�ֲ�����
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
		
		//�¶ȳ�������ʱ���� 
		if(temperature>=700)	//����70
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_2);//���ⱨ��
			TIM_SetCompare1(TIM1, 0);
			TIM_SetCompare4(TIM1, 50);
		}
		if(temperature<=300)//����30
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_2);
			TIM_SetCompare1(TIM1, 500);
			TIM_SetCompare4(TIM1, 100);
		}
		else
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_2);//���ⱨ��
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
					LCD_Fill(30+54,50,256,80,WHITE);//ɾ�� 
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
