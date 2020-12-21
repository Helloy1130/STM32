#include "sim900a.h"
#include "usart.h"		
#include "delay.h"		 	 	 	 
#include "lcd.h" 	 
#include "touch.h" 	 
#include "spi.h"

extern u8 TXData[4];
extern u8 send_flag;			//�����ź�

void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
//	unsigned long foffset; 
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
	
	
	
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)
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



//////////////////////////////////////////////////////////////////////////////////	   

u16 time=0;

//�������
const u8* kbd_tbl1[14]={"1","2","3","4","5","6","7","8","9","*","0","#","DEL","SEND"};
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
	if(i<14)Show_Str_Mid(x+(i%3)*80,y+6+28*(i/3),(u8*)kbd_tbl[i],16,80);
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
//	u8 *p,*p1,*p2,*s0,*s1,*s2,*s3,*s4,*s5,*s6,*s7,*s8,*s9;
	
	u8 pohnenumlen=0;		//���볤��,���15���� 
	u8 phonebuf[20]; 		//���뻺��
	u8 timex=0;
	u8 key=0;
	u8 smssendsta=0;		//���ŷ���״̬,0,�ȴ�����;1,����ʧ��;2,���ͳɹ� 
	u16 receive_data = 0xffff;

	LCD_Clear(WHITE);  
	POINT_COLOR=RED;

	POINT_COLOR=BLUE;

	sim900a_load_keyboard(0,180,(u8**)kbd_tbl1);//��ʾ���� 
	LCD_ShowString(10,130,200,16,16,"Receive from FPGA:");
			
	while(1)
	{		
		key=sim900a_get_keynum(0,180);
		
		receive_data = SPI4_Write_Read(0xFFFF);
		printf("%d\r\n",receive_data);
		LCD_ShowNum(150,130,receive_data,4,16);
		
		if(key)
		{   
			if(smssendsta)
			{
				smssendsta=0;
			}
			if(key<10||key==11)
			{
				if(pohnenumlen<15)
				{ 
					phonebuf[pohnenumlen]=kbd_tbl[key-1][0];
					if(key==11)
					{
						TXData[pohnenumlen] = 0;
					}
					if(key<10)
					{
						TXData[pohnenumlen] = key;
					}
				}
				pohnenumlen++;
			}
			else
			{
				if(key==13)
				{
					if(pohnenumlen) 
					{
						pohnenumlen--;
					}
					LCD_Fill(30+54,20,256,50,WHITE);//ɾ�� 
					TXData[pohnenumlen] = 99;
				}  
				if(key==14&&pohnenumlen)			
				{  
					send_flag = 1;
				}
				if(key==15) {}
				if(send_flag&&pohnenumlen>4)
				{
					LCD_ShowString(30+54,50,156,24,16,"DATA ERROR!"); 
					send_flag = 0;
					return ;
				}
			} 
			phonebuf[pohnenumlen]=0; 
			LCD_Fill(30+54,20,156,24,WHITE);
			Show_Str(30+54,20,156,24,phonebuf,24,0); 	
		}
		if((timex%20)==0)
		timex++;
		delay_ms(10);
		if(send_flag == 1)
		{
			return;
		}
		receive_data = 0xffff;
	} 
}













