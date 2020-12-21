#include "sim900a.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "lcd.h" 	  	 
#include "touch.h"
#include "string.h"
#include "math.h"
#include "spi.h"

/*******************************************
DDS_mode:
			1:��Ƶ
			2:AM;
			3:FM;
			4:PSK
			5:ASK
			6:����VPP
*******************************************/
u8 DDS_mode;
u8 DDS_mode1;//�ݴ�

//Ƶ��
u32 Frequency = 0;

//AM���ƶ�10%-100%   --->   	SPI���ݣ�1-10
u32 am = 10;

//FM���ƶ�Ƶƫ5kHz 	---> 	SPI���ݣ�1 	Ƶƫ10kHz ---> SPI���ݣ�2
u32 fm = 2;

//����VPP,10%-100%	--->   	SPI���ݣ�1-10
u32 vpp = 100;

extern u8 FRE_change_flag;//Ƶ�ʲ����˵ı�־
extern u8 AM_change_flag;//AM���ƶȲ����˵ı�־
extern u8 FM_change_flag;//FMƵƫ�ı��־
extern u8 VPP_change_flag;//VPP�ı��־

//��ָ��λ�ÿ�ʼ��ʾһ���ַ���	    
//֧���Զ�����
//(x,y):��ʼ����
//width,height:����
//str  :�ַ���
//size :�����С
//mode:0,�ǵ��ӷ�ʽ;1,���ӷ�ʽ    	   		   
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	   	    				    				  	  
    while(*str!=0)//����δ����
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

//�������
const u8* kbd_tbl1[15]={"1","2","3","4","5","6","7","8","9",".","0","DEL"," ","C"," "};
//�������
const u8* kbd_tbl2[7]={"AM","FM","PSK","ASK","FRE","VPP","OK"};
u8** kbd_tbl;
//���ؼ��̽��棨�ߴ�Ϊ450*300��
void sim900a_load_keyboard(u16 x,u16 y,u8 **kbtbl)
{
	u16 i;
	POINT_COLOR=RED;
	kbd_tbl=kbtbl;
	//���ּ���
	LCD_Fill(x,y,x+450,y+300,WHITE);
	LCD_DrawRectangle(x,y,x+450,y+300);						   
	LCD_DrawRectangle(x+150,y,x+300,y+300);						   
	LCD_DrawRectangle(x,y+60,x+450,y+240);
	LCD_DrawRectangle(x,y+120,x+450,y+180);
	
	POINT_COLOR=BLUE;
	for(i=0;i<15;i++)
	{
		Show_Str_Mid(x+(i%3)*150,y+12+60*(i/3),(u8*)kbd_tbl[i],24,150);
	} 
	
	//����ģʽ
	LCD_Fill(x,y+320,x+150,y+400,YELLOW);
	Show_Str_Mid(x,y+332,(u8*)kbd_tbl2[0],24,150);
	
	LCD_Fill(x+300,y+320,x+450,y+400,YELLOW);
	Show_Str_Mid(x+300,y+332,(u8*)kbd_tbl2[1],24,150);
	
	LCD_Fill(x,y+420,x+150,y+500,YELLOW);
	Show_Str_Mid(x,y+432,(u8*)kbd_tbl2[2],24,150);
	
	LCD_Fill(x+300,y+420,x+450,y+500,YELLOW);	
	Show_Str_Mid(x+300,y+432,(u8*)kbd_tbl2[3],24,150);
	
	LCD_Fill(x,y+520,x+150,y+600,YELLOW);
	Show_Str_Mid(x,y+532,(u8*)kbd_tbl2[4],24,150);
	
	LCD_Fill(x+300,y+520,x+450,y+600,YELLOW);
	Show_Str_Mid(x+300,y+532,(u8*)kbd_tbl2[5],24,150);
	
	LCD_Fill(x+160,y+560,x+290,y+640,GREEN);
	Show_Str_Mid(x+150,y+572,(u8*)kbd_tbl2[6],24,150);
								
}

//�õ�������������
//x,y:��������
//����ֵ��������ֵ��1~22��Ч��0,��Ч��
//����ֵ��1-9������1-9   10��.   11��0    12��DEL    14��C     16��AM    17��FM    18:PSK     19:ASK     20:��Ƶ       21������VPP     22��OK
u8 sim900a_get_keynum(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x=0;//0,û���κΰ������£�1~22��1~22�Ű�������
	u8 key=0;
	tp_dev.scan(0); 		 
	if(tp_dev.sta&TP_PRES_DOWN)			//������������
	{
		//���ּ��̱�����
		for(i=0;i<5;i++)
		{
			for(j=0;j<3;j++)
			{
			 	if(tp_dev.x[0]<(x+j*150+150)&&tp_dev.x[0]>(x+j*150)&&tp_dev.y[0]<(y+i*60+60)&&tp_dev.y[0]>(y+i*60))
				{	
					key=i*3+j+1;	
					break;	 		   
				}
			}
			if(key) break;
		}
		//ģʽ��������
		if(tp_dev.x[0]<165&&tp_dev.x[0]>15&&tp_dev.y[0]<550&&tp_dev.y[0]>470)	key=16;		
		if(tp_dev.x[0]<465&&tp_dev.x[0]>315&&tp_dev.y[0]<550&&tp_dev.y[0]>470)	key=17;
		if(tp_dev.x[0]<165&&tp_dev.x[0]>15&&tp_dev.y[0]<650&&tp_dev.y[0]>570)	key=18;
		if(tp_dev.x[0]<465&&tp_dev.x[0]>315&&tp_dev.y[0]<650&&tp_dev.y[0]>570)	key=19;
		if(tp_dev.x[0]<165&&tp_dev.x[0]>15&&tp_dev.y[0]<750&&tp_dev.y[0]>670)	key=20;	
		if(tp_dev.x[0]<465&&tp_dev.x[0]>315&&tp_dev.y[0]<750&&tp_dev.y[0]>670)	key=21;	
		if(tp_dev.x[0]<305&&tp_dev.x[0]>175&&tp_dev.y[0]<790&&tp_dev.y[0]>710)	key=22;	
		
		if(key_x==key) key=0;//��ֹ��δ���
		else key_x=key;		
	}
	else if(key_x) key_x=0;
	
	return key; 
}

//����ģʽ���������� 
void sim900a_sms_send_test(void)
{
	u8 i,j;
	u32 fre; 		
	u8 frebuf[20]; 			//Ƶ����ʾ
	u8 phonebuf[20]; 		//���ݻ���
	u8 pohnenumlen=0;		//���ݳ��ȣ����8λ  
	u8 timex=0;
	u8 key=0;
	
	LCD_Clear(WHITE);
	
	LCD_ShowString(10,10,150,24,24,"MODE:");	
	LCD_ShowString(10,40,150,24,24,"SET(Hz):");
	LCD_ShowString(10,70,200,24,24,"Frequency(Hz):");	
	LCD_ShowString(150,10,150,24,24,"AM(%):");
	LCD_ShowString(300,10,150,24,24,"FM(kHz):");
	LCD_ShowString(300,40,150,24,24,"VPP(%):");
	sim900a_load_keyboard(15,150,(u8**)kbd_tbl1);//��ʾ���� 
	
	while(1)
	{
		key=sim900a_get_keynum(15,150);
		if(key)
		{   
			if(key<10||key==11)
			{
				if(pohnenumlen<15)
				{ 
					phonebuf[pohnenumlen++]=kbd_tbl1[key-1][0];
				}
			}
			else
			{
				if(key==12) if(pohnenumlen) pohnenumlen--;//ɾ��
				if(key==14) if(pohnenumlen) pohnenumlen=0;//���
				if(key==16) {DDS_mode1 = 2;LCD_Fill(100,10,150,34,WHITE);LCD_ShowString(80,10,100,24,24,"AM");}
				if(key==17) {DDS_mode1 = 3;LCD_Fill(100,10,150,34,WHITE);LCD_ShowString(80,10,100,24,24,"FM");}
				if(key==18) {DDS_mode1 = 4;LCD_Fill(100,10,150,34,WHITE);LCD_ShowString(80,10,100,24,24,"PSK");}
				if(key==19) {DDS_mode1 = 5;LCD_Fill(100,10,150,34,WHITE);LCD_ShowString(80,10,100,24,24,"ASK");}
				if(key==20) {DDS_mode1 = 1;LCD_Fill(100,10,150,34,WHITE);LCD_ShowString(80,10,100,24,24,"FRE");}				
				if(key==21) {DDS_mode1 = 6;LCD_Fill(100,10,150,34,WHITE);LCD_ShowString(80,10,100,24,24,"VPP");}	
				if(key==22)				//ȷ��
				{  
					DDS_mode = DDS_mode1;
					SPI1_Write_cmd(DDS_mode);
					if(DDS_mode==1)//��Ƶ
					{
						if(pohnenumlen<9&&pohnenumlen>0)
						{
							fre = 0;
							for(i=0;i<=pohnenumlen-1;i++)
							{
								fre = fre * 10 + (phonebuf[i]-48);
							}
							Frequency = fre;
							LCD_ShowString(10,124,200,24,24,"Frequency Setted!");
							LCD_Fill(200,70,480,104,WHITE);
							Show_Str(200,70,200,24,phonebuf,24,0);
							delay_ms(1000);
							LCD_Fill(10,124,480,148,WHITE);
						}
						else
						{
							LCD_ShowString(10,124,100,24,24,"ERROR!");
							delay_ms(1000);
							LCD_Fill(10,124,150,148,WHITE);
						}
						SPI1_Write_data(Frequency);
					}
					if(DDS_mode==2)//AM
					{
						am = 10;//Ĭ��
						SPI1_Write_data(am/10);
						LCD_ShowString(10,124,200,24,24,"AM Setted!");
						LCD_Fill(220,10,300,34,WHITE);
						LCD_ShowNum(220,10,am,3,24);
						delay_ms(1000);
						LCD_Fill(10,124,480,148,WHITE);
					}
					if(DDS_mode==3)//FM
					{
						fm = 2;//Ĭ��10KƵƫ
						SPI1_Write_data(fm);
						LCD_ShowString(10,124,200,24,24,"FM Setted!");
						LCD_Fill(400,10,480,34,WHITE);
						LCD_ShowNum(400,10,10,2,24);
						delay_ms(1000);
						LCD_Fill(10,124,480,148,WHITE);
					}
					if(DDS_mode==4)//PSK
					{
						LCD_ShowString(10,124,200,24,24,"PSK Setted!");
						delay_ms(1000);
						LCD_Fill(10,124,480,148,WHITE);
					}
					if(DDS_mode==5)//ASK
					{
						LCD_ShowString(10,124,200,24,24,"ASK Setted!");
						delay_ms(1000);
						LCD_Fill(10,124,480,148,WHITE);
					}
					if(DDS_mode==6)//VPP
					{
						vpp = 100;//Ĭ��
						SPI1_Write_data(vpp/10);
						LCD_ShowString(10,124,200,24,24,"VPP Setted!");
						LCD_Fill(400,40,480,64,WHITE);
						LCD_ShowNum(400,40,vpp,3,24);
						delay_ms(1000);
						LCD_Fill(10,124,480,148,WHITE);
					}
				}
			} 
			phonebuf[pohnenumlen]=0; //д�������
			LCD_Fill(120,40,280,64,WHITE);
			Show_Str(120,40,200,24,phonebuf,24,0); 
		}
		//Ƶ�ʲ���
		if(FRE_change_flag)
		{
			FRE_change_flag = 0;
			SPI1_Write_data(Frequency);
			fre = Frequency;
			for(i=0;i<8;i++)
			{
				if((u8)(Frequency/(pow(10,7-i)))!=0)
				{
					for(j=0;j<8-i;j++)
					{
						frebuf[j] = fre/(pow(10,7-i-j)) + 48;
						fre = fre%(u32)(pow(10,7-i-j));
					}
					frebuf[j] = 0;//д�������
					break;
				}
			}
			LCD_Fill(200,70,400,104,WHITE);
			Show_Str(200,70,200,24,frebuf,24,0); 
			delay_ms(10);
		}
		//AM���ƶȲ���
		if(AM_change_flag)
		{
			AM_change_flag = 0;
			SPI1_Write_data(am/10);
			LCD_Fill(220,10,300,34,WHITE);
			LCD_ShowNum(220,10,am,3,24);
		}
		//FMƵƫ5kHz/10kHz
		if(FM_change_flag)
		{
			FM_change_flag = 0;
			SPI1_Write_data(fm);
			LCD_Fill(400,10,480,34,WHITE);
			LCD_ShowNum(400,10,fm*5,2,24);
		}
		//VPP����
		if(VPP_change_flag)
		{
			VPP_change_flag = 0;
			SPI1_Write_data(vpp/10);
			LCD_Fill(400,40,480,64,WHITE);
			LCD_ShowNum(400,40,vpp,3,24);
		}
		if((timex%50)==0)LED0=!LED0;//500ms��˸ 
		timex++;
		delay_ms(10);
	}
} 


