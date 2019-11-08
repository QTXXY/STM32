//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//
//
//
//  �� �� ��   : oled.c
//  �� �� ��   : v1.0
//  ��    ��   :
//  ��������   : 2019-0704
//  ����޸�   : 
//  ��������   : 0.69��7��OLED �ӿ���ʾ����(STM32F103C8T6ϵ��IIC)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND   ��Դ��
//              VCC   ��5V��3.3v��Դ
//              SCL   ��PB6��SCL��
//              SDA   ��PB7��SDA�� 
//              RES   ��PB5 �������4��iic�ӿ�����ſ��Բ��� �ߵ�ƽ��Ч
//              DC    ��GND
//              CS    ��GND
//              ----------------------------------------------------------------
//
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

#include "oled.h"
#include "delay.h"
#include "usart.h"
#include "codetab.h"

/*********************OLEDд����************************************/ 
void OLED_WrDat(unsigned char IIC_Data)
{
	Start();
	WriteByte(0x78);
	if (WaitAck()) return;
	WriteByte(0x40);			//write data
	if (WaitAck()) return;
	WriteByte(IIC_Data);
	if (WaitAck()) return;
	Stop();
}

/*********************OLEDд����************************************/
void OLED_WrCmd(unsigned char IIC_Command)
{
	Start();
	WriteByte(0x78);    //���SA0(��DC����)Ϊ�͵�ƽ��OLED�ӻ���ַΪ0x78����SA0Ϊ�ߵ�ƽ���ӻ���ַΪ0x7A
	if (WaitAck()) return;
	WriteByte(0x00);	//write command
	if (WaitAck()) return;
	WriteByte(IIC_Command);
	if (WaitAck()) return;
	Stop();
	
}

/*********************OLED ��������************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WrCmd(0xb0+y);
	OLED_WrCmd(((x&0xf0)>>4)|0x10);
	OLED_WrCmd((x&0x0f)|0x01);
}

/*********************OLED����************************************/
void OLED_Fill(unsigned char bmp_dat) 
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		OLED_WrDat(bmp_dat);
	}
}

/*********************OLED��λ************************************/
void OLED_CLS(void)
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		OLED_WrDat(0);
	}
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//�õ�ƫ�ƺ��ֵ			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(Char_Size ==16)
		{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WrDat(F8X16[c*16+i]);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WrDat(F8X16[c*16+i+8]);
		}
		else
		{	
			OLED_Set_Pos(x,y);
			for(i=0;i<6;i++)
			OLED_WrDat(F6x8[c][i]);				
		}
}

//��ʾһ���ַ��Ŵ�
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}

//m^n����
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  


/****************************************************************
*���ܣ���ʾ����
*
*������x,yΪ��ʼ��ʾ����,x:0~127 y:0~7 len�����ֵ�λ��  size:12 or 16
*
*      num ��ʾ������(0~4294967295)    mode(��):ģʽ	0,���ģʽ;1,����ģʽ
*
*****************************************************************/
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 


/****************************************************************
*���ܣ���ʾ8*6һ���׼ASCII�ַ���
*
*������x,yΪ��ʼ��ʾ���꣬x��Χ0-127(128��)��y��Χ 0-7(8��)��chΪ��ʾ�ַ�
*
*****************************************************************/
void OLED_P6x8Str(uchar x,uchar y,unsigned char ch[])
{
	uchar c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		OLED_WrDat(F6x8[c][i]);
		x+=6;
		j++;
	}
}

/****************************************************************
*���ܣ���ʾ8*16һ���׼ASCII�ַ���
*
*������x,yΪ��ʼ��ʾ���꣬x��Χ0-127(128��)��y��Χ 0-7(8��)��chΪ��ʾ�ַ�
*
*****************************************************************/
void OLED_P8x16Str(uchar x,uchar y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<8;i++)
		OLED_WrDat(F8X16[c*16+i]);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		OLED_WrDat(F8X16[c*16+i+8]);
		x+=8;
		j++;
	}
}
/****************************************************************
*���ܣ���ʾ16*16�ĵ���
*
*������x,yΪ��ʼ��ʾ���꣬x��Χ0-127(128��)��y��Χ 0-7(8��)��NΪ��������
*
* Nһ����ȡģ�����������
*
*****************************************************************/
void OLED_P16x16Ch(uchar x,uchar y,uchar N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	} 	  	
}
/**********************************************************************
*���ܣ���ʾͼƬ
*
*������x0,y0Ϊ��ʼ��ʾ���꣬x0��Χ0-127(128��)��y0��Χ 0-7(8��)��BMPΪͼƬ����
*
* x1,y1Ϊ��ʾ����x���Ϊ128��y���Ϊ8,������ȫ����ʾͼƬ
*
************************************************************************/
void Draw_BMP(uchar x0,uchar y0,uchar x1,uchar y1,uchar BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WrDat(BMP[j++]);
	    }
	}
}

void InitOled(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //PB.5 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	GPIO_SetBits(GPIOB,GPIO_Pin_5);	                         //PB5����ߵ�ƽ����RES

	DelayMs(500);

	OLED_WrCmd(0xae);//--turn off oled panel
	OLED_WrCmd(0x00);//---set low column address
	OLED_WrCmd(0x10);//---set high column address
	OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WrCmd(0x81);//--set contrast control register
	OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
	OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WrCmd(0xa6);//--set normal display
	OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3f);//--1/64 duty
	OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x00);//-not offset
	OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WrCmd(0xd9);//--set pre-charge period
	OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WrCmd(0xda);//--set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb);//--set vcomh
	OLED_WrCmd(0x40);//Set VCOM Deselect Level
	OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x02);//
	OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
	OLED_WrCmd(0x14);//--set(0x10) disable
	OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WrCmd(0xaf);//--turn on oled panel

	OLED_Fill(0x00); //��ʼ����
	OLED_Set_Pos(0,0);
}




