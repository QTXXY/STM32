#ifndef _LCD12864_H_
#define _LCD12864_H_

#include "sys.h"
#define  uchar unsigned char
#define  uint unsigned int
	
#define byte uchar

extern void SendInstruct(uchar instruct);
extern void SendData(uchar data);
extern void LcdDisplayString(byte y, byte x,char * string);
extern void ClearLcdDisplay(void);
extern void InitLcd(void);

#endif
