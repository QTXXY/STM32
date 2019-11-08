//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//
//
//
//  文 件 名   : lcd12864.c
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


#include "lcd12864.h"
#include "delay.h"

#define PinClk	   	PBout(15)  // EN
#define PinCs	  	PBout(14)  // RS
#define PinData	   	PBout(13)  // R/W
#define PinReset	PBout(12)  // RST

#define GetBit(data, offset)        (((data) >> (offset)) & 0x01)   // 获取位

const char Array[] = 
{

0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,    //第一行汉字位置  
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,    //第二行汉字位置  
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,    //第三行汉字位置 
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,    //第四行汉字位置 
};

void SendInstruct(uchar instruct) //发送指令
{
    short i;
    uint variable;
	
	//0x00F80000是一个启动传输指令的指令，只不过是启动字节与指令进行合并
	variable = 0x00F80000 | ((instruct & 0xF0) << 8) |((instruct & 0x0F) << 4);
	
    PinClk = 0;    //时钟线默认状态下是低电平
    PinCs = 1;     // CS为高，开始传输
	
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
	
	//0x00FA0000是一个启动传输数据的指令，只不过是启动字节与数据进行合并
    variable = 0x00FA0000 | ((data & 0xF0) << 8) |((data & 0x0F) << 4); 

    PinClk = 0;    //时钟线默认状态下是低电平
    PinCs = 1;     // CS为高，开始传输

    for (i = 23; i > -1; i--) 
    {
        if (GetBit(variable, i)) //获取位，先发送高位再发送低位
            PinData = 1;
        else 
            PinData = 0;
        DelayUs(10);
        PinClk = 1;//形成一个上升沿，数据开始发送
        DelayUs(10);
        PinClk = 0;//恢复到下降沿，准备下一次发送数据
        DelayUs(10);
    }
    PinCs = 0;
}

/*******************************************************************************
* 描述	    : Lcd屏显示函数
* 输入参数  : y: 行号0、1、2、3
*           : x: 列号 0-15
*           : string 字符串
*******************************************************************************/
void LcdDisplayString(byte y, byte x,char * string)
{     	
    SendInstruct(Array[8*y + x]);  //发送指令，发送行号
	while(*string != '\0')
	{
		SendData(*string);
		string++;
	}      
}

void ClearLcdDisplay(void)//清屏
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
	PinReset = 1;   //由0->1,禁止复位
	
    SendInstruct(0x30);   //功能设定:0x30表明后续指令为基本指令 0x34为扩展指令集
    SendInstruct(0x0C);   //显示状态为开
    SendInstruct(0x01);   //清除显示
    SendInstruct(0x02);   //地址归0
    SendInstruct(0x80);   //
    SendInstruct(0x01);   //	
}




