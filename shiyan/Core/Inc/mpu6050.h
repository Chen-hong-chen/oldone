#ifndef _MPU6050_H_
#define _MPU6050_H_
#include "main.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"

#include "main.h"
#include "i2c.h"



typedef struct MPU6050
{
   int16_t mpuax;
   int16_t mpuay;
   int16_t mpuaz;
   int16_t mpugx;
   int16_t mpugy;
   int16_t mpugz;
}MPU,*mpu;



//计算角度
//typedef struct Angle
//{
//    double X_Angle;
//    double Y_Angle;
//    double Z_Angle;
//    
//} MPU6050_Angle;

#define X_ACCEL_OFFSET -600 
#define Y_ACCEL_OFFSET -100 
#define Z_ACCEL_OFFSET 2900 
#define X_GYRO_OFFSET 32 
#define Y_GYRO_OFFSET -11 
#define Z_GYRO_OFFSET 1
 
#define MPU6050_I2C      		hi2c1
#define MPU6050_ADDR     		0xD0   
#define I2C_TimeOut  			100
 
/*MPU6050内部寄存器地址*/
#define MPU_SAMPLE_RATE_REG		0X19	//采样频率分频器
#define MPU_GYRO_CFG_REG		0X1B	//陀螺仪配置寄存器
#define MPU_ACCEL_CFG_REG		0X1C	//加速度计配置寄存器
#define MPU_ACCEL_XOUTH_REG		0X3B	//加速度值,X轴高8位寄存器
#define MPU_TEMP_OUTH_REG		0X41	//温度值高八位寄存器
#define MPU_GYRO_XOUTH_REG		0X43	//陀螺仪值,X轴高8位寄存器
#define MPU_PWR_MGMT1_REG		0X6B	//电源管理寄存器1
#define MPU_DEVICE_ID_REG		0X75	//器件ID寄存器
 
uint8_t MPU6050_Init(I2C_HandleTypeDef *I2Cx);
float MPU_Get_Temperature(void);
uint8_t MPU_Get_RAW_Gyroscope(int16_t *gx,int16_t *gy,int16_t *gz);
uint8_t MPU_Get_RAW_Accelerometer(int16_t *ax,int16_t *ay,int16_t *az);
void Mpu6050_Task(void const * argument);
//void MPU6050_Get_Angle(MPU6050_Angle  data,MPU ta);

#endif