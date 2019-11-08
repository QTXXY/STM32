#ifndef _IIC_H_
#define _IIC_H_

#include "sys.h"

typedef unsigned char       byte;
typedef unsigned short      ushort;


//PB7 SDA��I/O����
#define SdaIn()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;} //��������������
#define SdaOut() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;} //�������

//IO��������	 
#define PinScl       PBout(6) //SCL
#define PinSda    	 PBout(7) //SDA	 
#define ReadSda      PBin(7)  //����SDA 

#define SlaveNoAck   2        //�ӻ���Ӧ��
//#define MemoryAddress 0xA0    //�ڴ�ӻ���ַ
//#define DeviceAddress 0xD0    //����ӻ���ַ



void InitIic(void);
void Start(void);
void Stop(void);
u8 WaitAck(void);
void SendAck(void);
void SendNAck(void);
void WriteByte(u8 data);
u8 ReadByte(void);
u8 MasterWriteMemory(unsigned char DeviceAddr,unsigned short RegisterAddr,unsigned short count, unsigned char *DataBuffer);
u8 MasterReadMemory(unsigned char DeviceAddr,unsigned short RegisterAddr,unsigned short count, unsigned char *DataBuffer);
u8 MasterWriteDevice(unsigned char DeviceAddr,unsigned char RegisterAddr,unsigned char count, unsigned char *DataBuffer);
u8 MasterReadDevice(unsigned char DeviceAddr,unsigned char RegisterAddr,unsigned char count, unsigned char *DataBuffer);

#endif
