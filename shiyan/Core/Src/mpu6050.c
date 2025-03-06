#include "mpu6050.h"
#include "math.h"
MPU MPU6050;//data

uint8_t MPU6050_Init(I2C_HandleTypeDef *I2Cx)
{
	uint8_t check;
	uint8_t Data;
 
	// check device ID WHO_AM_I
	HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR, MPU_DEVICE_ID_REG, 1, &check, 1, I2C_TimeOut);
	// 0x68 will be returned by the sensor if everything goes well
	if (check == 104) 
	{
			// power management register 0X6B we should write all 0's to wake the sensor up
			Data = 0;
			HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR, MPU_PWR_MGMT1_REG, 1, &Data, 1, I2C_TimeOut);
 
			// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
			Data = 0x07;
			HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR, MPU_SAMPLE_RATE_REG, 1, &Data, 1, I2C_TimeOut);
 
			// Set accelerometer configuration in ACCEL_CONFIG Register
			// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 ->   2g
			Data = 0x00;
			HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR, MPU_ACCEL_CFG_REG, 1, &Data, 1, I2C_TimeOut);
 
			// Set Gyroscopic configuration in GYRO_CONFIG Register
			// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 ->   250  /s
			Data = 0x00;
			HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR, MPU_GYRO_CFG_REG, 1, &Data, 1, I2C_TimeOut);
			return 0;
	}
	return 1;
}
 
 
/**
* @brief 		MPU6050温度值获取函数
* @alter		无
* @param		无
* @retval 		温度值
*/
float MPU_Get_Temperature(void)
{
	uint8_t buf[2]; 
	short raw;
	float temp;
	HAL_I2C_Mem_Read(&MPU6050_I2C, MPU6050_ADDR, MPU_TEMP_OUTH_REG, 1, buf, 2, I2C_TimeOut);
	raw=((int16_t)buf[0]<<8)|buf[1];  
	temp=36.53+((double)raw)/340;  
	return temp;;
}
 
/**
* @brief 		MPU6050陀螺仪值获取函数(三轴原始值)
* @alter		无
* @param		gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
* @retval 		正常：0，错误：其他
*/
uint8_t MPU_Get_RAW_Gyroscope(int16_t*gx,int16_t *gy,int16_t*gz)
{
	uint8_t buf[6],res;  
	res = HAL_I2C_Mem_Read(&MPU6050_I2C, MPU6050_ADDR, MPU_GYRO_XOUTH_REG, 1, buf, 6, I2C_TimeOut);
	if(res==0)
	{
		*gx=((int16_t)buf[0]<<8)|buf[1];  
		*gy=((int16_t)buf[2]<<8)|buf[3];  
		*gz=((int16_t)buf[4]<<8)|buf[5];
	} 	
    return res;
}
 
/**
* @brief 		MPU6050加速度值获取函数(三轴原始值)
* @alter		无
* @param		ax,ay,az:加速度计x,y,z轴的原始读数(带符号)
* @retval 		正常：0，错误：其他
*/
uint8_t MPU_Get_RAW_Accelerometer(int16_t *ax,int16_t *ay,int16_t *az)
{
	uint8_t buf[6],res;  
	res = HAL_I2C_Mem_Read(&MPU6050_I2C, MPU6050_ADDR, MPU_ACCEL_XOUTH_REG, 1, buf, 6, I2C_TimeOut);
	if(res==0)
	{
		*ax=((int16_t)buf[0]<<8)|buf[1];  
		*ay=((int16_t)buf[2]<<8)|buf[3];  
		*az=((int16_t)buf[4]<<8)|buf[5];
	} 	
    return res;

}
        
    


/****************************************************************************** 
* 函数介绍： 计算 x, y, z 三轴的倾角 
* 输入参数： 无 
* 输出参数： data：角度结构体 
* 返回值 ： 无 
******************************************************************************/
//void MPU6050_Get_Angle(MPU6050_Angle data,MPU ta)
//{   
//    /* 计算x, y, z 轴倾角，返回弧度值*/
//    data.X_Angle = acos(30000 / 16384);
//    data.Y_Angle = acos((double)((ta.mpuay+ Y_ACCEL_OFFSET) / 16384));
//    data.Z_Angle = acos((double)((ta.mpuaz+ Z_ACCEL_OFFSET) / 16384));
//
//    /* 弧度值转换为角度值 */
//    data.X_Angle = data.X_Angle * 57.29577;
//    data.Y_Angle = data.Y_Angle * 57.29577;
//    data.Z_Angle = data.Z_Angle * 57.29577;
//} 



void Mpu6050_Task(void const * argument)
{
  /* USER CODE BEGIN Mpu6050_Task */
  /* Infinite loop */
//   while(MPU6050_Init(&hi2c1)==1)
//  {
//   
//    //printf("MPU init is ok\r\n");
//    
//    
//  }
  for(;;)
  {
//    MPU_Get_RAW_Accelerometer(&MPU6050.mpuax,&MPU6050.mpuay,&MPU6050.mpuaz);
//    MPU_Get_RAW_Gyroscope(&MPU6050.mpugx,&MPU6050.mpugy,&MPU6050.mpugz);
//    if((abs(MPU6050.mpuax)/163>80)||(abs(MPU6050.mpuay)/163>80))
//    {
//     printf("gx:%d  gy:%d    \r\n",MPU6050.mpuax/163,MPU6050.mpuay/163);
//    }
    osDelay(500);
  }
  /* USER CODE END Mpu6050_Task */
}
