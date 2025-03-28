#include "pubdata.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "chaochengbo.h"
#include "usart.h"
#include "mpu6050.h"
#include "stdlib.h"
#include "W25Qx.h"
#include <string.h>
#include "tim.h"
#include "pubdata.h"
#include "rc522.h"
#include "gps.h"
#include "ec800_at.h"
extern osMutexId Mutex_ziyuanHandle;
extern int light_buff;
gps  gps_buff;//gps的结构体
old_guai_zhang oldone;//拐杖的数据库
gongneng_set hw_ctrl[SHUKA_F+1]=
{
   [LIGHT_F]=light_gongnneg_set,
   [TIME_F]=time_gongnneg_set,
   [SHUKA_F]=suaka_gongnneg_set,

};


int set_database(old_guai_flag index,void * param)//  修改数据库值
{
        if(param == NULL || index > RUN)
      {
          printf("set_database error\r\n");
          return -1;
      }
   old_guai_zhang_param *p = param;
   xSemaphoreTake(Mutex_ziyuanHandle, portMAX_DELAY);//互斥量保护资源
   switch(index)
   {
       case GPS_F:     //gps 精度
           oldone.gps_data.jingdu = p->gps_data.jingdu;
           oldone.gps_data.weidu = p->gps_data.weidu;
           //printf("shuju_jingdu = %.12f\r\nweidu = %.12f\r\n",oldone.gps_data.jingdu,oldone.gps_data.weidu);
           break;
       case MPU_F:           //是否发生倾斜
           oldone.mpu= p->mpu;
          // printf("oldone.mpu = %d\r\n",oldone.mpu);
           break;
       case CHAOSHENG_F:         //距离是否小于0.5米
           oldone.chaosheng = p->chaosheng;
         //  printf("oldone.chaosheng = %d\r\n",oldone.chaosheng);
           break;
       case LIGHT_F:               //是否开手电筒
           oldone.light = p->light;
           if(hw_ctrl[LIGHT_F])
               hw_ctrl[LIGHT_F](oldone.light);
         //  printf("light = %d\r\n",oldone.light);
           break;
       case TIME_F:                 //是否开启时间播报功能
           oldone.time = p->time;
           if(hw_ctrl[TIME_F])
               hw_ctrl[TIME_F](oldone.time);
          //  printf("time = %d\r\n",oldone.time);
           break;
       case XINHAO_T:                   //手机信号强度
           oldone.xinhao = p->xinhao;
           break;
      case AIR_F:                 //天气情况
           oldone.tianqi.tianqi1= p->tianqi.tianqi1;
           oldone.tianqi.wendu= p->tianqi.wendu;
          // printf("shuju_tianqi = %d\r\nwendu = %d\r\n",oldone.tianqi.tianqi1,oldone.tianqi.wendu);
           break;
      case SHUKA_F:               //是否开启刷卡鉴权功能
           oldone.shuaka = p->shuaka;
           if(hw_ctrl[SHUKA_F])
               hw_ctrl[SHUKA_F](oldone.shuaka);
          // printf("shuaka = %d\r\n",oldone.shuaka);
           break;
      case DINGGSHI_TIMER:               //设置定时运动的时间
           oldone.dingshitimer = p->dingshitimer;
         //  printf("dingshitimer = %d\r\n",oldone.dingshitimer);
           break;
      case DINGSHI:               //设置是否定时运动
           oldone.Dingshi = p->Dingshi;
          // printf("Dingshi = %d\r\n",oldone.Dingshi);
           break;
      case RUN:               //是否运行
           oldone.run = p->run;
          // printf("run = %d\r\n",oldone.run);
           break;
               
       default:
           ;
   }
   
   xSemaphoreGive(Mutex_ziyuanHandle);
  return 0;
}
int get_database(old_guai_flag index,void * param)//  获取数据库的值
{
          if(param == NULL || index > RUN)
      {
          printf("set_database error\r\n");
          return -1;
      }
   old_guai_zhang_param *p = param;
   xSemaphoreTake(Mutex_ziyuanHandle, portMAX_DELAY);//互斥量保护资源
   switch(index)
   {
       case GPS_F:     //gps 精度
            p->gps_data.jingdu=oldone.gps_data.jingdu;
            p->gps_data.weidu=oldone.gps_data.weidu;
           break;
       case MPU_F:           //是否发生倾斜
           p->mpu=oldone.mpu;
           break;
       case CHAOSHENG_F:         //距离是否小于0.5米
           p->chaosheng = oldone.chaosheng;
           break;
       case LIGHT_F:               //是否开手电筒
           p->light = oldone.light;
           break;
       case TIME_F:                 //是否开启时间播报功能
           p->time=oldone.time;
           break;
       case XINHAO_T:                   //手机信号强度
          p->xinhao = oldone.xinhao;
           break;
      case AIR_F:                 //天气情况
           p->tianqi.tianqi1=oldone.tianqi.tianqi1;
           p->tianqi.wendu=oldone.tianqi.wendu;
           break;
      case SHUKA_F:               //是否开启刷卡鉴权功能
           p->shuaka = oldone.shuaka;
           break;
      case DINGGSHI_TIMER:               //是否开启刷卡鉴权功能
           p->dingshitimer=oldone.dingshitimer;
           break;
      case DINGSHI:               //是否开启刷卡鉴权功能
           p->Dingshi=oldone.Dingshi;
           break; 
      case RUN:               //是否运行
           p->run=oldone.run;
           
           break;
       default:
           ;
   }
   
   xSemaphoreGive(Mutex_ziyuanHandle);
  return 0;
}
int light_gongnneg_set(uint8_t status)
{
  if(status==ON)
  {
       light_buff=ON;
       HAL_GPIO_WritePin(ROAD_LED_GPIO_Port, ROAD_LED_Pin, GPIO_PIN_SET);//灯亮
  }
  else
  {
       light_buff=ZE;
       HAL_GPIO_WritePin(ROAD_LED_GPIO_Port, ROAD_LED_Pin, GPIO_PIN_RESET);//灯灭
  }
  
  return 0;
}

int time_gongnneg_set(uint8_t status)
{
  
  
  return 0;
}

int suaka_gongnneg_set(uint8_t status)
{
  
  
  return 0;
}
