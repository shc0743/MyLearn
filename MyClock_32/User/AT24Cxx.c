#include "stm32f10x.h"                  // Device header

#include "delay.h"

#include "AT24Cxx.h"

// IIC函数
static void IIC_Init(void);   // 初始化IIC的IO口				 
static void IIC_Start(void);   // 发送IIC开始信号
static void IIC_Stop(void);   // 发送IIC停止信号
static void IIC_Send_Byte(u8 txd);   // IIC发送一个字节
static u8 IIC_Read_Byte(u8 ack);   // IIC读取一个字节
static u8 IIC_Wait_Ack(void);   // IIC等待ACK信号
static void IIC_Ack(void);   // IIC发送ACK信号
static void IIC_NAck(void);   // IIC不发送ACK信号

#define IIC_SCL_PIN GPIO_Pin_6
#define IIC_SDA_PIN GPIO_Pin_7
#define IIC_SCL_PORT GPIOB
#define IIC_SDA_PORT GPIOB
#define READ_SDA GPIO_ReadInputDataBit(IIC_SDA_PORT, IIC_SDA_PIN)


/*******************************************************************************
* 函 数 名         : IIC_Init
* 函数功能		   : IIC初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void IIC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//RCC_APB2PeriphClockCmd(,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=IIC_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(IIC_SCL_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
	
	//GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN);
	//GPIO_SetBits(IIC_SDA_PORT, IIC_SDA_PIN);	
	GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN);
	GPIO_SetBits(IIC_SDA_PORT, IIC_SDA_PIN);
}
/*******************************************************************************
* 函 数 名         : SDA_OUT
* 函数功能		   : SDA输出配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
}

/*******************************************************************************
* 函 数 名         : SDA_IN
* 函数功能		   : SDA输入配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
}

/*******************************************************************************
* 函 数 名         : IIC_Start
* 函数功能		   : 产生IIC起始信号   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	GPIO_SetBits(IIC_SDA_PORT, IIC_SDA_PIN);	  	  
	GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN);
	delay_us(5);
 	GPIO_ResetBits(IIC_SDA_PORT, IIC_SDA_PIN);//START:when CLK is high,DATA change form high to low 
	delay_us(6);
	GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN);//钳住I2C总线，准备发送或接收数据 
}	

/*******************************************************************************
* 函 数 名         : IIC_Stop
* 函数功能		   : 产生IIC停止信号   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN);
	GPIO_ResetBits(IIC_SDA_PORT, IIC_SDA_PIN);//STOP:when CLK is high DATA change form low to high
 	GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN); 
	delay_us(6); 
	GPIO_SetBits(IIC_SDA_PORT, IIC_SDA_PIN);//发送I2C总线结束信号
	delay_us(6);							   	
}

/*******************************************************************************
* 函 数 名         : IIC_Wait_Ack
* 函数功能		   : 等待应答信号到来   
* 输    入         : 无
* 输    出         : 1，接收应答失败
        			 0，接收应答成功
*******************************************************************************/
static u8 IIC_Wait_Ack(void)
{
	u8 tempTime=0;
	SDA_IN();      //SDA设置为输入  
	GPIO_SetBits(IIC_SDA_PORT, IIC_SDA_PIN);
	delay_us(1);	   
	GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN);
	delay_us(1);	 
	while(READ_SDA)
	{
		tempTime++;
		if(tempTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN);//时钟输出0 	   
	return 0;  
} 

/*******************************************************************************
* 函 数 名         : IIC_Ack
* 函数功能		   : 产生ACK应答  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void IIC_Ack(void)
{
	GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN);
	SDA_OUT();
	GPIO_ResetBits(IIC_SDA_PORT, IIC_SDA_PIN);
	delay_us(2);
	GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN);
	delay_us(5);
	GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN);
}

/*******************************************************************************
* 函 数 名         : IIC_NAck
* 函数功能		   : 产生NACK非应答  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/		    
static void IIC_NAck(void)
{
	GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN);
	SDA_OUT();
	GPIO_SetBits(IIC_SDA_PORT, IIC_SDA_PIN);
	delay_us(2);
	GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN);
	delay_us(5);
	GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN);
}	

/*******************************************************************************
* 函 数 名         : IIC_Send_Byte
* 函数功能		   : IIC发送一个字节 
* 输    入         : txd：发送一个字节
* 输    出         : 无
*******************************************************************************/		  
static void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        if((txd&0x80)>0) //0x80  1000 0000
			GPIO_SetBits(IIC_SDA_PORT, IIC_SDA_PIN);
		else
			GPIO_ResetBits(IIC_SDA_PORT, IIC_SDA_PIN);
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN);
		delay_us(2); 
		GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN);	
		delay_us(2);
    }	 
} 

/*******************************************************************************
* 函 数 名         : IIC_Read_Byte
* 函数功能		   : IIC读一个字节 
* 输    入         : ack=1时，发送ACK，ack=0，发送nACK 
* 输    出         : 应答或非应答
*******************************************************************************/  
static u8 IIC_Read_Byte(u8 ack)
{
	u8 i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN); 
        delay_us(2);
		GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN);
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}
/*******************************************************************************
* 函 数 名         : AT24CXX_Init
* 函数功能		   : AT24CXX初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void AT24CXX_Init(void)
{
	IIC_Init();//IIC初始化
}

/*******************************************************************************
* 函 数 名         : AT24CXX_ReadOneByte
* 函数功能		   : 在AT24CXX指定地址读出一个数据
* 输    入         : ReadAddr:开始读数的地址 
* 输    出         : 读到的数据
*******************************************************************************/
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址	    
	}
	else 
	{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据
	} 	   
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}

/*******************************************************************************
* 函 数 名         : AT24CXX_WriteOneByte
* 函数功能		   : 在AT24CXX指定地址写入一个数据
* 输    入         : WriteAddr  :写入数据的目的地址 
					 DataToWrite:要写入的数据
* 输    出         : 无
*******************************************************************************/
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}
	else 
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据
	} 	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

/*******************************************************************************
* 函 数 名         : AT24CXX_WriteLenByte
* 函数功能		   : 在AT24CXX里面的指定地址开始写入长度为Len的数据
					 用于写入16bit或者32bit的数据
* 输    入         : WriteAddr  :写入数据的目的地址 
					 DataToWrite:要写入的数据
					 Len        :要写入数据的长度2,4
* 输    出         : 无
*******************************************************************************/
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,unsigned int Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/*******************************************************************************
* 函 数 名         : AT24CXX_ReadLenByte
* 函数功能		   : 在AT24CXX里面的指定地址开始读出长度为Len的数据
					 用于读出16bit或者32bit的数据
* 输    入         : ReadAddr   :开始读出的地址 
					 Len        :要读出数据的长度2,4
* 输    出         : 读取的数据
*******************************************************************************/
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/*******************************************************************************
* 函 数 名         : AT24CXX_Check
* 函数功能		   : 检查AT24CXX是否正常
* 输    入         : 无
* 输    出         : 1:检测失败，0:检测成功
*******************************************************************************/
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0x36)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X36);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X36)return 0;
	}
	return 1;											  
}

/*******************************************************************************
* 函 数 名         : AT24CXX_Read
* 函数功能		   : 在AT24CXX里面的指定地址开始读出指定个数的数据
* 输    入         : ReadAddr :开始读出的地址 对24c02为0~255
					 pBuffer  :数据数组首地址
					 NumToRead:要读出数据的个数
* 输    出         : 无
*******************************************************************************/
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
} 

/*******************************************************************************
* 函 数 名         : AT24CXX_Write
* 函数功能		   : 在AT24CXX里面的指定地址开始写入指定个数的数据
* 输    入         : WriteAddr :开始写入的地址 对24c02为0~255
					 pBuffer  :数据数组首地址
					 NumToRead:要读出数据的个数
* 输    出         : 无
*******************************************************************************/
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}









