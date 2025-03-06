#ifndef _PUBDATA_H_
#define _PUBDATA_H_

#include "stm32f1xx_hal.h"

#define    ON     1
#define   AIR_LENGTH   20
#define   ZE     0
#define  ERROR   -1
#define EC80_RECEIVE_LEN  800


typedef uint64_t  u64;
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef int8_t  s8;

typedef struct 
{
  float jingdu;  //gps 精度
  float weidu;   //gps 维度
}gps;//gps的结构体
typedef struct 
{
  gps  gps_data;
  int mpu;       //是否发生倾斜
  int chaosheng;   //距离是否小于0.5米
  int light;       //是否开手电筒
  int time;       //是否开启时间播报功能
  int xinhao;      //手机信号强度
  char  air[AIR_LENGTH];        //天气情况
  int shuaka;     //是否开启刷卡鉴权功能
}old_guai_zhang;//上传MQTT的结构体

typedef union
{
  gps  gps_data;
  int mpu;       //是否发生倾斜
  int chaosheng;   //距离是否小于0.5米
  int light;       //是否开手电筒
  int time;       //是否开启时间播报功能
  int xinhao;      //手机信号强度
  char  air[AIR_LENGTH];        //天气情况
  int shuaka;     //是否开启刷卡鉴权功能
}old_guai_zhang_param;//上传MQTT的结构体


typedef enum 
{
   GPS_F,              //gps 
   MPU_F,       //是否发生倾斜
   CHAOSHENG_F,   //距离是否小于0.5米
   LIGHT_F,       //是否开手电筒
   TIME_F,       //是否开启时间播报功能
   XINHAO_T,      //手机信号强度
    AIR_F,        //天气情况
   SHUKA_F    //是否开启刷卡鉴权功能
}old_guai_flag;//上传MQTT的结构体




typedef struct
{
  uint16_t rx_len;
  uint8_t receive_flag;
  uint8_t usartDMA_rxBuf[EC80_RECEIVE_LEN];
}EC80ReceiveType;//4G的接受串口数据

typedef int (*gongneng_set)(uint8_t status);

int set_database(old_guai_flag index,void * param);//  修改数据库值
int get_database(old_guai_flag index,void * param);//  获取数据库的值

//功能函数
int light_gongnneg_set(uint8_t status);
int time_gongnneg_set(uint8_t status);
int suaka_gongnneg_set(uint8_t status);



//回调函数

#endif