#ifndef _OLED_H_
#define _OLED_H_

#include "iic.h"

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64

#define	Brightness	0xCF  //¡¡∂»
#define X_WIDTH 	128
#define Y_WIDTH 	64

#define  uchar unsigned char
#define  uint unsigned int

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size);
u32 oled_pow(u8 m,u8 n);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2);

void OLED_WrDat(unsigned char IIC_Data);
void OLED_WrCmd(unsigned char IIC_Command);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char bmp_dat);
void OLED_CLS(void);
void OLED_P6x8Str(uchar x,uchar y,unsigned char ch[]);
void OLED_P16x16Ch(uchar x,uchar y,uchar N);
void OLED_P8x16Str(uchar x,uchar y,unsigned char ch[]);
void Draw_BMP(uchar x0,uchar y0,uchar x1,uchar y1,uchar BMP[]);
void InitOled(void);


#endif
