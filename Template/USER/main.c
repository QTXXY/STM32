//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//
//
//
//  �� �� ��   : main.c
//  �� �� ��   : v1.0
//  ��    ��   :
//  ��������   : 2019-0707
//  ����޸�   : 
//  ��������   : ���з�ʽ����LCD12864
//
//              ˵��: 
//              ----------------------------------------------------------------
//              GND����GND
//              VCC����VCC(5V or 3.3V)
//              V0 ������(���߽�һ���ɱ���赽VCC)
//              RS ������PB14
//              RW ������PB13
//              E  ������PB15
//              DB0-DB7 ������
//              PSB������GND  ����ģʽ
//              RST������PB12
//              BLA����VCC(5V or 3.3V)
//              BLK������GND
//              ʣ�����Ų��ӣ�����
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

static float Data = 9876.65; //��12864��ʾ
char Table[] = {"0000.00"};


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	InitDelay();
	InitUart(115200);
	InitLcd();
	
	ClearLcdDisplay();
	
	Table[0] = ((int)(Data * 100)/100000 + '0');     //ǧ
	Table[1] = ((int)(Data * 100)/10000%10 + '0');   //��
	Table[2] = ((int)(Data * 100)/1000%10 + '0');    //ʮ
	Table[3] = ((int)(Data * 100)/100%10 + '0');     //��
	Table[4] = '.';                                  //С����
	Table[5] = ((int)(Data * 100)/10%10+'0');        //С�����һλ
	Table[6] = ((int)(Data * 100)%10+'0');           //С�������λ
	while(1)
	{		
		LcdDisplayString(0,0,"��ʱ���º�ʱ��");
		LcdDisplayString(1,0,"���ﳤ����δ��");
		LcdDisplayString(2,0,"��ʹ���Ƿɽ���");
		LcdDisplayString(3,0,"���̺������ɽ");
		DelayMs(1800);                               //�����ʱ���ܳ���2000
		ClearLcdDisplay();
		DelayMs(100);
		LcdDisplayString(0,2,"��ʾ������");           //��0��
		LcdDisplayString(1,10,Table);
		DelayMs(1800);
//		ClearLcdDisplay();                            //�ɲ�Ҫ�����ֻḲ�ǵ�
//		DelayMs(100);
	}	 
} 

