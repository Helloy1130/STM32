#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "iwdg.h"


//在配置看门狗后， DS0 将常亮，如果 WK_UP 按键按下，就喂狗，只要 WK_UP 不停的按，
//看门狗就一直不会产生复位，保持 DS0 的常亮，一旦超过看门狗定溢出时间（Tout）还没按，
//那么将会导致程序重启，这将导致 DS0 熄灭一次。

int main(void)
{	
	delay_init();
	LED_Init();
	KEY_Init();
	
	delay_ms(300);   	 //让人看得到灭
	
	IWDG_Init(4,625);    //与分频数为64,重载值为625,溢出时间为1s
	LED0=0;				 //点亮LED0
	
	while(1)
	{
		if(KEY_Scan(0)==WK_UP_PRES)	IWDG_Feed();//如果WK_UP按下,则喂狗
		delay_ms(10);
	}
}
