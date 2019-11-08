#include "iic.h"
#include "delay.h"

//ʵ�ʲ��ԣ�����SCL����:181K

void InitIic(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7); 	//PB6,PB7 �����
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

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 WaitAck(void)
{
	u8 OverTime = 0;
	SdaIn();
	PinSda = 1;DelayUs(1);
	PinScl = 1;DelayUs(1);
	
	while(ReadSda)
	{
		OverTime++;
		if(OverTime > 250)    //��ʱ��Ӧ��
		{
			Stop();
			return 1;
		}
	}
	PinScl = 0;
	return 0;
}

//����ACK�ź�
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
		if((data & 0x80) >> 7)  //��ȡ����λ
			PinSda = 1;
		else
			PinSda = 0;
		
		data <<= 1;
		DelayUs(2);
		PinScl = 1;               //����SCL����������
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
*  ����		:	������FM3116���ڴ����
*  �������	:	DeviceAddr	�ӻ���ַ			RegisterAddr	�Ĵ�����ַ
*            	count		д�����ݸ���		DataBuffer		д�����ݵ�ַ
*  ��������	:	0			ͨ�ųɹ�			2				ͨ��ʧ�ܣ��ӻ�NACK
*****************************************************************************/
u8 MasterWriteMemory(unsigned char DeviceAddr,unsigned short RegisterAddr,unsigned short count, unsigned char *DataBuffer)
{
	unsigned char SlaveAddrWrite;        //�ӻ���ַ+W
//	unsigned char SlaveAddrRead;         //�ӻ���ַ+R
	unsigned char HighRegAddr;           //���ֽڼĴ�����ַ
	unsigned char LowRegAddr;            //���ֽڼĴ�����ַ
	
	SlaveAddrWrite = DeviceAddr & 0xFE;
	
	HighRegAddr = (u8)(RegisterAddr >> 8);
	LowRegAddr  = (u8)(RegisterAddr);
	
	Start();                               //����IIC
	WriteByte(SlaveAddrWrite);                 //���ʹӻ���ַ
	if(WaitAck())	return SlaveNoAck;     //�ӻ���Ӧ��
	
	WriteByte(HighRegAddr);                //���ͼĴ�����ַ���ֽ�
	if(WaitAck())	return SlaveNoAck;

	WriteByte(LowRegAddr);                 //���ͼĴ�����ַ���ֽ�
	if(WaitAck())	return SlaveNoAck;
	
	while(count--)
	{
		WriteByte(*DataBuffer);            //����д��count���ֽ�
		DataBuffer++;                      //���ݵ�ַ����
		if(WaitAck())	return SlaveNoAck;
	}
	
	Stop();                                //����ֹͣ�ź�
	
	return 0;
}

/*****************************************************************************
*  ����		:	������FM3116���ڴ����
*  �������	:	DeviceAddr	�ӻ���ַ			RegisterAddr	�Ĵ�����ַ
*            	count		��ȡ���ݸ���		DataBuffer		�������ݵ�ַ
*  ��������	:	0			ͨ�ųɹ�			2				ͨ��ʧ�ܣ��ӻ�NACK
*****************************************************************************/
u8 MasterReadMemory(unsigned char DeviceAddr,unsigned short RegisterAddr,unsigned short count, unsigned char *DataBuffer)
{
	unsigned char SlaveAddrWrite;        //�ӻ���ַ+W
	unsigned char SlaveAddrRead;         //�ӻ���ַ+R
	unsigned char HighRegAddr;           //���ֽڼĴ�����ַ
	unsigned char LowRegAddr;            //���ֽڼĴ�����ַ
	
	SlaveAddrWrite = DeviceAddr & 0xFE;
	SlaveAddrRead  = DeviceAddr | 0x01;
	
	HighRegAddr = (u8)(RegisterAddr >> 8);
	LowRegAddr  = (u8)(RegisterAddr);
	
	Start();                               //����IIC
	WriteByte(SlaveAddrWrite);             //���ʹӻ���ַ+W	
	if(WaitAck())	return SlaveNoAck;
	
	WriteByte(HighRegAddr);                //���ͼĴ�����ַ���ֽ�
	if(WaitAck())	return SlaveNoAck;

	WriteByte(LowRegAddr);                 //���ͼĴ�����ַ���ֽ�
	if(WaitAck())	return SlaveNoAck;	
	
	Start();                               
	WriteByte(SlaveAddrRead);              //���ʹӻ���ַ+R	
	if(WaitAck())	return SlaveNoAck;
	
	while(count--)
	{
		*DataBuffer = ReadByte();         //���մӻ�������
		DataBuffer++;                     //���ջ�������ַ����
		SendAck();                        //���ӻ�һ��Ack
	}
	
	SendNAck();                           //���ݽ������
	Stop();
	return 0;
}
/*****************************************************************************
*  ����		:	������FM3116���豸����
*  �������	:	DeviceAddr	�ӻ���ַ			RegisterAddr	�Ĵ�����ַ
*            	count		д�����ݸ���		DataBuffer		д�����ݵ�ַ
*  ��������	:	0			ͨ�ųɹ�			2				ͨ��ʧ�ܣ��ӻ�NACK
*****************************************************************************/
u8 MasterWriteDevice(unsigned char DeviceAddr,unsigned char RegisterAddr,unsigned char count, unsigned char *DataBuffer)
{
	unsigned char SlaveAddrWrite;            //�ӻ���ַ+W
//	unsigned char SlaveAddrRead;             //�ӻ���ַ+R
	
	SlaveAddrWrite = DeviceAddr & 0xFE;
	
	Start();                                //����IIC
	WriteByte(SlaveAddrWrite);              //���ʹӻ���ַ+W	
	if(WaitAck())	return SlaveNoAck;      //�ȴ��ӻ���Ӧ

	WriteByte(RegisterAddr);                //���ͼĴ�����ַ
	if(WaitAck())	return SlaveNoAck;

	while(count--)
	{
		WriteByte(*DataBuffer);            //����д��count���ֽ�
		DataBuffer++;                      //���ݵ�ַ����
		if(WaitAck())	return SlaveNoAck;
	}
	
	Stop();                                //����ֹͣ�ź�
	
	return 0;
}

/*****************************************************************************
*  ����		:	������FM3116���豸����
*  �������	:	DeviceAddr	�ӻ���ַ			RegisterAddr	�Ĵ�����ַ
*            	count		��ȡ���ݸ���		DataBuffer		�������ݵ�ַ
*  ��������	:	0			ͨ�ųɹ�			2				ͨ��ʧ�ܣ��ӻ�NACK
*****************************************************************************/
u8 MasterReadDevice(unsigned char DeviceAddr,unsigned char RegisterAddr,unsigned char count, unsigned char *DataBuffer)
{
	unsigned char SlaveAddrWrite;            //�ӻ���ַ+W
	unsigned char SlaveAddrRead;             //�ӻ���ַ+R	
	
	SlaveAddrWrite = DeviceAddr & 0xFE;
	SlaveAddrRead  = DeviceAddr | 0x01;
	
	Start();                               //����IIC
	WriteByte(SlaveAddrWrite);             //���ʹӻ���ַ+W	
	if(WaitAck())	return SlaveNoAck;     //�ȴ��ӻ���Ӧ
	
	WriteByte(RegisterAddr);                //���ͼĴ�����ַ
	if(WaitAck())	return SlaveNoAck;
	
	Start();                               
	WriteByte(SlaveAddrRead);              //���ʹӻ���ַ+R	
	if(WaitAck())	return SlaveNoAck;
	
	while(count--)
	{
		*DataBuffer = ReadByte();         //���մӻ�������
		DataBuffer++;                     //���ջ�������ַ����
		SendAck();                        //���ӻ�һ��Ack
	}
	
	SendNAck();                           //���ݽ������
	Stop();
	return 0;	
}

