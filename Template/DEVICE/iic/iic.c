#include "iic.h"
#include "delay.h"

//实际测试，传输SCL速率:181K

void InitIic(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7); 	//PB6,PB7 输出高
}


void Start(void)
{
	SdaOut();
	PinSda = 1;
	PinScl = 1;
	DelayUs(4);
	PinSda = 0;
	DelayUs(4);
	PinScl = 0;
}

void Stop(void)
{
	SdaOut();
	PinScl = 0;
	PinSda = 0;
	DelayUs(4);
	PinScl = 1;
	PinSda = 1;
	DelayUs(4);
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 WaitAck(void)
{
	u8 OverTime = 0;
	SdaIn();
	PinSda = 1;DelayUs(1);
	PinScl = 1;DelayUs(1);
	
	while(ReadSda)
	{
		OverTime++;
		if(OverTime > 250)    //超时无应答
		{
			Stop();
			return 1;
		}
	}
	PinScl = 0;
	return 0;
}

//产生ACK信号
void SendAck(void)
{
	PinScl = 0;
	SdaOut();
	PinSda = 0;
	DelayUs(2);
	PinScl = 1;
	DelayUs(2);
	PinScl = 0;

}

void SendNAck(void)
{
	PinScl = 0;
	SdaOut();
	PinSda = 1;
	DelayUs(2);
	PinScl = 1;
	DelayUs(2);
	PinScl = 0;
}

void WriteByte(u8 data)
{
	u8 i;
	SdaOut();
	PinScl = 0;                  
	for (i = 0; i < 8; i++)
	{
		if((data & 0x80) >> 7)  //获取发送位
			PinSda = 1;
		else
			PinSda = 0;
		
		data <<= 1;
		DelayUs(2);
		PinScl = 1;               //拉高SCL，传输数据
		DelayUs(2);
		PinScl = 0;
		DelayUs(2);
	}
}


u8 ReadByte(void)
{
	u8 i,ReceiveData = 0;
	
	SdaIn();
	for (i = 0; i < 8; i++)
	{
		PinScl = 0;
		DelayUs(2);
		PinScl = 1;
		
		ReceiveData <<= 1;
		if(ReadSda) ReceiveData++;
		DelayUs(1);
	}
	
	return ReceiveData;
}

/*****************************************************************************
*  描述		:	主机对FM3116的内存访问
*  输入参数	:	DeviceAddr	从机地址			RegisterAddr	寄存器地址
*            	count		写入数据个数		DataBuffer		写入数据地址
*  返回数据	:	0			通信成功			2				通信失败，从机NACK
*****************************************************************************/
u8 MasterWriteMemory(unsigned char DeviceAddr,unsigned short RegisterAddr,unsigned short count, unsigned char *DataBuffer)
{
	unsigned char SlaveAddrWrite;        //从机地址+W
//	unsigned char SlaveAddrRead;         //从机地址+R
	unsigned char HighRegAddr;           //高字节寄存器地址
	unsigned char LowRegAddr;            //低字节寄存器地址
	
	SlaveAddrWrite = DeviceAddr & 0xFE;
	
	HighRegAddr = (u8)(RegisterAddr >> 8);
	LowRegAddr  = (u8)(RegisterAddr);
	
	Start();                               //启动IIC
	WriteByte(SlaveAddrWrite);                 //发送从机地址
	if(WaitAck())	return SlaveNoAck;     //从机无应答
	
	WriteByte(HighRegAddr);                //发送寄存器地址高字节
	if(WaitAck())	return SlaveNoAck;

	WriteByte(LowRegAddr);                 //发送寄存器地址低字节
	if(WaitAck())	return SlaveNoAck;
	
	while(count--)
	{
		WriteByte(*DataBuffer);            //连续写入count个字节
		DataBuffer++;                      //数据地址递增
		if(WaitAck())	return SlaveNoAck;
	}
	
	Stop();                                //发送停止信号
	
	return 0;
}

/*****************************************************************************
*  描述		:	主机对FM3116的内存访问
*  输入参数	:	DeviceAddr	从机地址			RegisterAddr	寄存器地址
*            	count		读取数据个数		DataBuffer		保存数据地址
*  返回数据	:	0			通信成功			2				通信失败，从机NACK
*****************************************************************************/
u8 MasterReadMemory(unsigned char DeviceAddr,unsigned short RegisterAddr,unsigned short count, unsigned char *DataBuffer)
{
	unsigned char SlaveAddrWrite;        //从机地址+W
	unsigned char SlaveAddrRead;         //从机地址+R
	unsigned char HighRegAddr;           //高字节寄存器地址
	unsigned char LowRegAddr;            //低字节寄存器地址
	
	SlaveAddrWrite = DeviceAddr & 0xFE;
	SlaveAddrRead  = DeviceAddr | 0x01;
	
	HighRegAddr = (u8)(RegisterAddr >> 8);
	LowRegAddr  = (u8)(RegisterAddr);
	
	Start();                               //启动IIC
	WriteByte(SlaveAddrWrite);             //发送从机地址+W	
	if(WaitAck())	return SlaveNoAck;
	
	WriteByte(HighRegAddr);                //发送寄存器地址高字节
	if(WaitAck())	return SlaveNoAck;

	WriteByte(LowRegAddr);                 //发送寄存器地址低字节
	if(WaitAck())	return SlaveNoAck;	
	
	Start();                               
	WriteByte(SlaveAddrRead);              //发送从机地址+R	
	if(WaitAck())	return SlaveNoAck;
	
	while(count--)
	{
		*DataBuffer = ReadByte();         //接收从机的数据
		DataBuffer++;                     //接收缓冲区地址递增
		SendAck();                        //给从机一个Ack
	}
	
	SendNAck();                           //数据接收完毕
	Stop();
	return 0;
}
/*****************************************************************************
*  描述		:	主机对FM3116的设备访问
*  输入参数	:	DeviceAddr	从机地址			RegisterAddr	寄存器地址
*            	count		写入数据个数		DataBuffer		写入数据地址
*  返回数据	:	0			通信成功			2				通信失败，从机NACK
*****************************************************************************/
u8 MasterWriteDevice(unsigned char DeviceAddr,unsigned char RegisterAddr,unsigned char count, unsigned char *DataBuffer)
{
	unsigned char SlaveAddrWrite;            //从机地址+W
//	unsigned char SlaveAddrRead;             //从机地址+R
	
	SlaveAddrWrite = DeviceAddr & 0xFE;
	
	Start();                                //启动IIC
	WriteByte(SlaveAddrWrite);              //发送从机地址+W	
	if(WaitAck())	return SlaveNoAck;      //等待从机响应

	WriteByte(RegisterAddr);                //发送寄存器地址
	if(WaitAck())	return SlaveNoAck;

	while(count--)
	{
		WriteByte(*DataBuffer);            //连续写入count个字节
		DataBuffer++;                      //数据地址递增
		if(WaitAck())	return SlaveNoAck;
	}
	
	Stop();                                //发送停止信号
	
	return 0;
}

/*****************************************************************************
*  描述		:	主机对FM3116的设备访问
*  输入参数	:	DeviceAddr	从机地址			RegisterAddr	寄存器地址
*            	count		读取数据个数		DataBuffer		保存数据地址
*  返回数据	:	0			通信成功			2				通信失败，从机NACK
*****************************************************************************/
u8 MasterReadDevice(unsigned char DeviceAddr,unsigned char RegisterAddr,unsigned char count, unsigned char *DataBuffer)
{
	unsigned char SlaveAddrWrite;            //从机地址+W
	unsigned char SlaveAddrRead;             //从机地址+R	
	
	SlaveAddrWrite = DeviceAddr & 0xFE;
	SlaveAddrRead  = DeviceAddr | 0x01;
	
	Start();                               //启动IIC
	WriteByte(SlaveAddrWrite);             //发送从机地址+W	
	if(WaitAck())	return SlaveNoAck;     //等待从机响应
	
	WriteByte(RegisterAddr);                //发送寄存器地址
	if(WaitAck())	return SlaveNoAck;
	
	Start();                               
	WriteByte(SlaveAddrRead);              //发送从机地址+R	
	if(WaitAck())	return SlaveNoAck;
	
	while(count--)
	{
		*DataBuffer = ReadByte();         //接收从机的数据
		DataBuffer++;                     //接收缓冲区地址递增
		SendAck();                        //给从机一个Ack
	}
	
	SendNAck();                           //数据接收完毕
	Stop();
	return 0;	
}

