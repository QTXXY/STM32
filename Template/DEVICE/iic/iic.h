#ifndef _IIC_H_
#define _IIC_H_

#include "sys.h"

typedef unsigned char       byte;
typedef unsigned short      ushort;


//PB7 SDA的I/O方向
#define SdaIn()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;} //上拉、下拉输入
#define SdaOut() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;} //推挽输出

//IO操作函数	 
#define PinScl       PBout(6) //SCL
#define PinSda    	 PBout(7) //SDA	 
#define ReadSda      PBin(7)  //输入SDA 

#define SlaveNoAck   2        //从机无应答
//#define MemoryAddress 0xA0    //内存从机地址
//#define DeviceAddress 0xD0    //外设从机地址



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
