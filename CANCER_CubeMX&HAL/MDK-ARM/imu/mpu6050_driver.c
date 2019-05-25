#include "mpu6050_driver.h"

mpu_data_t     mpu_data;
imu_data_t     imu;

/**
  * @brief   	MPU6050初始化函数
  * @param 		none
  * @retval   0则成功		其他则失败
  */
uint8_t MPU_Init(void)
{
	uint8_t res;
	
	
	IIC_Init();																													//初始化IIC总线
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);															//复位MPU6050
  delay_ms(100);
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);															//唤醒MPU6050 
	MPU_Write_Byte(MPU_GYRO_CFG_REG,3<<3);															//陀螺仪传感器,±2000dps
	MPU_Write_Byte(MPU_ACCEL_CFG_REG,0<<3);															//加速度传感器,±2g
	MPU_Set_Rate(50);																										//设置采样率50Hz
	MPU_Write_Byte(MPU_INT_EN_REG,0X00);																//关闭所有中断
	MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);															//I2C主模式关闭
	MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);																//关闭FIFO
	MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);															//INT引脚低电平有效
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
	if(res==MPU_ADDR)																										//器件ID正确
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);														//设置CLKSEL,PLL X轴为参考
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);														//加速度与陀螺仪都工作
		MPU_Set_Rate(50);																									//设置采样率为50Hz
 	}else return 1;
	return 0;
}

/**
  * @brief   	设置MPU6050的数字低通滤波器函数
  * @param 		lpf:数字低通滤波频率(Hz)
  * @retval   0则设置成功		其他则失败
  */
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU_CFG_REG,data);														//设置数字低通滤波器  
}

/**
  * @brief   	设置MPU6050的采样率函数(假定Fs=1KHz)
  * @param 		rate:4~1000(Hz)
  * @retval   0则设置成功		其他则失败
  */
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);											//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);																					//自动设置LPF为采样率的一半
}

/**
  * @brief   	温度获得函数
  * @param 		none
  * @retval   温度值(扩大了100倍)
  */
short MPU_Get_Temperature(void)
{
    uint8_t buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((uint16_t)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}
/**
  * @brief   	获取陀螺仪数据函数
  * @param 		gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
  * @retval   0则成功		其他则失败
  */
uint8_t mpu_buff[8];
uint8_t MPU_Get_Gyroscope(float* gx, float* gy, float* gz)
{
  uint8_t res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,mpu_buff);
	if(res==0)
	{
		mpu_data.gx = ((mpu_buff[0] << 8 | mpu_buff[1])+34);
		mpu_data.gy = ((mpu_buff[2] << 8 | mpu_buff[3])-30);
		mpu_data.gz = ((mpu_buff[4] << 8 | mpu_buff[5])+3);

		*gx   = (float)mpu_data.gx * 0.5f;
		*gy   = (float)mpu_data.gy * 0.5f;
		*gz   = (float)mpu_data.gz * 0.5f;
	} 	
  return res;;
}

int32_t gx_sum,gy_sum,gz_sum;
void mpu_offset_cal(void)
{
  int i;
  for (i = 0; i < 500; i++)
  {
    MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,mpu_buff);
		
		gx_sum += (mpu_buff[0] << 8 | mpu_buff[1]);
		gy_sum += (mpu_buff[2] << 8 | mpu_buff[3]);
		gz_sum += (mpu_buff[4] << 8 | mpu_buff[5]);

    delay_ms(2);
  }
  mpu_data.gx_offset = gx_sum	/ 500;
  mpu_data.gy_offset = gy_sum	/ 500;
  mpu_data.gz_offset = gz_sum	/ 500;
}

/**
  * @brief   	MPU6050 I2C写入len长数据函数
  * @param 		addr:器件地址
  * @param 		reg:寄存器地址
  * @param 		len:写入长度
  * @param 		buf:数据区
  * @retval   0则成功		其他则失败
  */
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i; 
    IIC_Start(); 
	IIC_Send_Byte((addr<<1)|0);																					//发送器件地址+写命令	
	if(IIC_Wait_Ack())																									//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
    IIC_Send_Byte(reg);																								//写寄存器地址
    IIC_Wait_Ack();																										//等待应答
	for(i=0;i<len;i++)
	{
		IIC_Send_Byte(buf[i]);																						//发送数据
		if(IIC_Wait_Ack())																								//等待ACK
		{
			IIC_Stop();	 
			return 1;		 
		}		
	}    
    IIC_Stop();	 
	return 0;	
} 
/**
  * @brief   	MPU6050 I2C读入len长数据函数
  * @param 		addr:器件地址
  * @param 		reg:寄存器地址
  * @param 		len:写入长度
  * @param 		buf:数据区
  * @retval   0则成功		其他则失败
  */
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
 	IIC_Start(); 
	IIC_Send_Byte((addr<<1)|0);																				//发送器件地址+写命令	
	if(IIC_Wait_Ack())																								//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
    IIC_Send_Byte(reg);																							//写寄存器地址
    IIC_Wait_Ack();																									//等待应答
    IIC_Start();
	IIC_Send_Byte((addr<<1)|1);																				//发送器件地址+读命令	
    IIC_Wait_Ack();																									//等待应答 
	while(len)
	{
		if(len==1)*buf=IIC_Read_Byte(0);																//读数据,发送nACK 
		else *buf=IIC_Read_Byte(1);																			//读数据,发送ACK  
		len--;
		buf++; 
	}    
    IIC_Stop();																											//产生一个停止条件 
	return 0;	
}

/**
  * @brief   	MPU6050 I2C写入一个字节长数据函数
  * @param 		reg:寄存器地址
  * @param 		data:数据
  * @retval   0则成功		其他则失败
  */
uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data) 				 
{ 
    IIC_Start(); 
	IIC_Send_Byte((MPU_ADDR<<1)|0);																		//发送器件地址+写命令	
	if(IIC_Wait_Ack())																								//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
    IIC_Send_Byte(reg);																							//写寄存器地址
    IIC_Wait_Ack();																									//等待应答 
	IIC_Send_Byte(data);																							//发送数据
	if(IIC_Wait_Ack())																								//等待ACK
	{
		IIC_Stop();	 
		return 1;		 
	}		 
    IIC_Stop();	 
	return 0;
}
/**
  * @brief   	MPU6050 I2C读入一个字节长数据函数
  * @param 		reg:寄存器地址
  * @retval   0则成功		其他则失败
  */
uint8_t MPU_Read_Byte(uint8_t reg)
{
	uint8_t res;
    IIC_Start(); 
	IIC_Send_Byte((MPU_ADDR<<1)|0);																		//发送器件地址+写命令	
	IIC_Wait_Ack();																										//等待应答 
    IIC_Send_Byte(reg);																							//写寄存器地址
    IIC_Wait_Ack();																									//等待应答
    IIC_Start();
	IIC_Send_Byte((MPU_ADDR<<1)|1);																		//发送器件地址+读命令	
    IIC_Wait_Ack();																									//等待应答 
	res=IIC_Read_Byte(0);																							//读取数据,发送nACK 
    IIC_Stop();																											//产生一个停止条件 
	return res;		
}


