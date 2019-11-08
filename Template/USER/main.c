//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//
//
//
//  文 件 名   : main.c
//  版 本 号   : v1.0
//  作    者   :
//  生成日期   : 2019-0707
//  最近修改   : 
//  功能描述   : 串行方式驱动LCD12864
//
//              说明: 
//              ----------------------------------------------------------------
//              GND——GND
//              VCC——VCC(5V or 3.3V)
//              V0 ——空(或者接一个可变电阻到VCC)
//              RS ——接PB14
//              RW ——接PB13
//              E  ——接PB15
//              DB0-DB7 ——空
//              PSB——接GND  串行模式
//              RST——接PB12
//              BLA——VCC(5V or 3.3V)
//              BLK——接GND
//              剩余引脚不接，留空
//              ----------------------------------------------------------------
//
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "iic.h"
#include "oled.h"
#include "bmp.h"
#include "lcd12864.h"

static float Data = 9876.65; //在12864显示
char Table[] = {"0000.00"};


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	InitDelay();
	InitUart(115200);
	InitLcd();
	
	ClearLcdDisplay();
	
	Table[0] = ((int)(Data * 100)/100000 + '0');     //千
	Table[1] = ((int)(Data * 100)/10000%10 + '0');   //百
	Table[2] = ((int)(Data * 100)/1000%10 + '0');    //十
	Table[3] = ((int)(Data * 100)/100%10 + '0');     //个
	Table[4] = '.';                                  //小数点
	Table[5] = ((int)(Data * 100)/10%10+'0');        //小数点后一位
	Table[6] = ((int)(Data * 100)%10+'0');           //小数点后两位
	while(1)
	{		
		LcdDisplayString(0,0,"清时明月汉时关");
		LcdDisplayString(1,0,"万里长征人未还");
		LcdDisplayString(2,0,"但使龙城飞将在");
		LcdDisplayString(3,0,"不教胡马度阴山");
		DelayMs(1800);                               //最大延时不能超过2000
		ClearLcdDisplay();
		DelayMs(100);
		LcdDisplayString(0,2,"显示浮点数");           //第0行
		LcdDisplayString(1,10,Table);
		DelayMs(1800);
//		ClearLcdDisplay();                            //可不要，文字会覆盖掉
//		DelayMs(100);
	}	 
} 

