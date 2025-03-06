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
  float jingdu;  //gps ����
  float weidu;   //gps ά��
}gps;//gps�Ľṹ��
typedef struct 
{
  gps  gps_data;
  int mpu;       //�Ƿ�����б
  int chaosheng;   //�����Ƿ�С��0.5��
  int light;       //�Ƿ��ֵ�Ͳ
  int time;       //�Ƿ���ʱ�䲥������
  int xinhao;      //�ֻ��ź�ǿ��
  char  air[AIR_LENGTH];        //�������
  int shuaka;     //�Ƿ���ˢ����Ȩ����
}old_guai_zhang;//�ϴ�MQTT�Ľṹ��

typedef union
{
  gps  gps_data;
  int mpu;       //�Ƿ�����б
  int chaosheng;   //�����Ƿ�С��0.5��
  int light;       //�Ƿ��ֵ�Ͳ
  int time;       //�Ƿ���ʱ�䲥������
  int xinhao;      //�ֻ��ź�ǿ��
  char  air[AIR_LENGTH];        //�������
  int shuaka;     //�Ƿ���ˢ����Ȩ����
}old_guai_zhang_param;//�ϴ�MQTT�Ľṹ��


typedef enum 
{
   GPS_F,              //gps 
   MPU_F,       //�Ƿ�����б
   CHAOSHENG_F,   //�����Ƿ�С��0.5��
   LIGHT_F,       //�Ƿ��ֵ�Ͳ
   TIME_F,       //�Ƿ���ʱ�䲥������
   XINHAO_T,      //�ֻ��ź�ǿ��
    AIR_F,        //�������
   SHUKA_F    //�Ƿ���ˢ����Ȩ����
}old_guai_flag;//�ϴ�MQTT�Ľṹ��




typedef struct
{
  uint16_t rx_len;
  uint8_t receive_flag;
  uint8_t usartDMA_rxBuf[EC80_RECEIVE_LEN];
}EC80ReceiveType;//4G�Ľ��ܴ�������

typedef int (*gongneng_set)(uint8_t status);

int set_database(old_guai_flag index,void * param);//  �޸����ݿ�ֵ
int get_database(old_guai_flag index,void * param);//  ��ȡ���ݿ��ֵ

//���ܺ���
int light_gongnneg_set(uint8_t status);
int time_gongnneg_set(uint8_t status);
int suaka_gongnneg_set(uint8_t status);



//�ص�����

#endif