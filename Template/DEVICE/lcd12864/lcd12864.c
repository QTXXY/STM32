//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//
//
//
//  �� �� ��   : lcd12864.c
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


#include "lcd12864.h"
#include "delay.h"

#define PinClk	   	PBout(15)  // EN
#define PinCs	  	PBout(14)  // RS
#define PinData	   	PBout(13)  // R/W
#define PinReset	PBout(12)  // RST

#define GetBit(data, offset)        (((data) >> (offset)) & 0x01)   // ��ȡλ

const char Array[] = 
{

0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,    //��һ�к���λ��  
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,    //�ڶ��к���λ��  
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,    //�����к���λ�� 
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,    //�����к���λ�� 
};

void SendInstruct(uchar instruct) //����ָ��
{
    short i;
    uint variable;
	
	//0x00F80000��һ����������ָ���ָ�ֻ�����������ֽ���ָ����кϲ�
	variable = 0x00F80000 | ((instruct & 0xF0) << 8) |((instruct & 0x0F) << 4);
	
    PinClk = 0;    //ʱ����Ĭ��״̬���ǵ͵�ƽ
    PinCs = 1;     // CSΪ�ߣ���ʼ����
	
    for (i = 23; i > -1; i--) 
    {
        if(GetBit(variable, i)) 
        {
            PinData = 1;
        } 
        else 
        {
            PinData = 0;
        }
        DelayUs(10);
        PinClk = 1;
        DelayUs(10);
        PinClk = 0;
        DelayUs(10);
    }
    PinCs = 0;
}

void SendData(uchar data)
{
    short i;
    uint variable;
	
	//0x00FA0000��һ�������������ݵ�ָ�ֻ�����������ֽ������ݽ��кϲ�
    variable = 0x00FA0000 | ((data & 0xF0) << 8) |((data & 0x0F) << 4); 

    PinClk = 0;    //ʱ����Ĭ��״̬���ǵ͵�ƽ
    PinCs = 1;     // CSΪ�ߣ���ʼ����

    for (i = 23; i > -1; i--) 
    {
        if (GetBit(variable, i)) //��ȡλ���ȷ��͸�λ�ٷ��͵�λ
            PinData = 1;
        else 
            PinData = 0;
        DelayUs(10);
        PinClk = 1;//�γ�һ�������أ����ݿ�ʼ����
        DelayUs(10);
        PinClk = 0;//�ָ����½��أ�׼����һ�η�������
        DelayUs(10);
    }
    PinCs = 0;
}

/*******************************************************************************
* ����	    : Lcd����ʾ����
* �������  : y: �к�0��1��2��3
*           : x: �к� 0-15
*           : string �ַ���
*******************************************************************************/
void LcdDisplayString(byte y, byte x,char * string)
{     	
    SendInstruct(Array[8*y + x]);  //����ָ������к�
	while(*string != '\0')
	{
		SendData(*string);
		string++;
	}      
}

void ClearLcdDisplay(void)//����
{
	SendInstruct(0x30);
    SendInstruct(0x01);  
}

void InitLcd(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	PinClk = 0;	PinCs = 0; PinData = 0; PinReset = 0;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	DelayMs(100);
	PinReset = 1;   //��0->1,��ֹ��λ
	
    SendInstruct(0x30);   //�����趨:0x30��������ָ��Ϊ����ָ�� 0x34Ϊ��չָ�
    SendInstruct(0x0C);   //��ʾ״̬Ϊ��
    SendInstruct(0x01);   //�����ʾ
    SendInstruct(0x02);   //��ַ��0
    SendInstruct(0x80);   //
    SendInstruct(0x01);   //	
}




