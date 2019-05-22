#ifndef __MPU6050_IIC_H
#define __MPU6050_IIC_H
#include "system.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
   	   		   
/* IO方向设置 */
#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PB9输入模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB9输出模式
/* IO操作函数 */	 
#define IIC_SCL    BIT_ADDR(GPIOB_BASE+20,6) 												//SCL
#define IIC_SDA    BIT_ADDR(GPIOB_BASE+20,7) 												//SDA	 
#define READ_SDA   BIT_ADDR(GPIOB_BASE+16,7)  											//输入SDA 

void IIC_Init(void);                																//初始化IIC的IO口				 
void IIC_Start(void);																								//发送IIC开始信号
void IIC_Stop(void);	  																						//发送IIC停止信号
void IIC_Send_Byte(u8 txd);																					//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);																//IIC读取一个字节
u8 IIC_Wait_Ack(void); 																							//IIC等待ACK信号
void IIC_Ack(void);																									//IIC发送ACK信号
void IIC_NAck(void);																								//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#endif
















