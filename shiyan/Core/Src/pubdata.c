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
gps  gps_buff;//gps�Ľṹ��
old_guai_zhang oldone;//���ȵ����ݿ�
gongneng_set hw_ctrl[SHUKA_F+1]=
{
   [LIGHT_F]=light_gongnneg_set,
   [TIME_F]=time_gongnneg_set,
   [SHUKA_F]=suaka_gongnneg_set,

};


int set_database(old_guai_flag index,void * param)//  �޸����ݿ�ֵ
{
        if(param == NULL || index > SHUKA_F)
      {
          printf("set_database error\r\n");
          return -1;
      }
   old_guai_zhang_param *p = param;
   xSemaphoreTake(Mutex_ziyuanHandle, portMAX_DELAY);//������������Դ
   switch(index)
   {
       case GPS_F:     //gps ����
           oldone.gps_data.jingdu = p->gps_data.jingdu;
           oldone.gps_data.weidu = p->gps_data.weidu;
           break;
       case MPU_F:           //�Ƿ�����б
           oldone.mpu= p->mpu;
           break;
       case CHAOSHENG_F:         //�����Ƿ�С��0.5��
           oldone.chaosheng = p->chaosheng;
           break;
       case LIGHT_F:               //�Ƿ��ֵ�Ͳ
           oldone.light = p->light;
           if(hw_ctrl[LIGHT_F])
               hw_ctrl[LIGHT_F](oldone.light);
           break;
       case TIME_F:                 //�Ƿ���ʱ�䲥������
           oldone.time = p->time;
           if(hw_ctrl[TIME_F])
               hw_ctrl[TIME_F](oldone.time);
           break;
       case XINHAO_T:                   //�ֻ��ź�ǿ��
           oldone.xinhao = p->xinhao;
           break;
      case AIR_F:                 //�������
           oldone.air[AIR_LENGTH-1]= p->air[AIR_LENGTH-1];
           break;
      case SHUKA_F:               //�Ƿ���ˢ����Ȩ����
           oldone.shuaka = p->shuaka;
           if(hw_ctrl[SHUKA_F])
               hw_ctrl[SHUKA_F](oldone.shuaka);
           break;
           
       default:
           ;
   }
   
   xSemaphoreGive(Mutex_ziyuanHandle);
  return 0;
}
int get_database(old_guai_flag index,void * param)//  ��ȡ���ݿ��ֵ
{
          if(param == NULL || index > SHUKA_F)
      {
          printf("set_database error\r\n");
          return -1;
      }
   old_guai_zhang_param *p = param;
   xSemaphoreTake(Mutex_ziyuanHandle, portMAX_DELAY);//������������Դ
   switch(index)
   {
       case GPS_F:     //gps ����
            p->gps_data.jingdu=oldone.gps_data.jingdu;
            p->gps_data.weidu=oldone.gps_data.weidu;
           break;
       case MPU_F:           //�Ƿ�����б
           p->mpu=oldone.mpu;
           break;
       case CHAOSHENG_F:         //�����Ƿ�С��0.5��
           p->chaosheng = oldone.chaosheng;
           break;
       case LIGHT_F:               //�Ƿ��ֵ�Ͳ
           p->light = oldone.light;
           break;
       case TIME_F:                 //�Ƿ���ʱ�䲥������
           p->time=oldone.time;
           break;
       case XINHAO_T:                   //�ֻ��ź�ǿ��
          p->xinhao = oldone.xinhao;
           break;
      case AIR_F:                 //�������
          p->air[AIR_LENGTH-1]= oldone.air[AIR_LENGTH-1];
           break;
      case SHUKA_F:               //�Ƿ���ˢ����Ȩ����
           p->shuaka = oldone.shuaka;
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
    
       HAL_GPIO_WritePin(ROAD_LED_GPIO_Port, ROAD_LED_Pin, GPIO_PIN_SET);//����
  }
  else
  {
    
       HAL_GPIO_WritePin(ROAD_LED_GPIO_Port, ROAD_LED_Pin, GPIO_PIN_RESET);//����
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
