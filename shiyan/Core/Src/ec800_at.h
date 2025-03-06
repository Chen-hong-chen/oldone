#ifndef _EC800_AT_H_
#define _EC800_AT_H_
#include <stdbool.h>
#define AT_SIZE   256
#define  http_url    "https://restapi.amap.com/v3/assistant/coordinate/convert?locations=116.481499,39.990475&coordsys=gps&output=json&key=bfe9091507c1bc660bef259200c3e2d2"


#define  ST_COUNT  25   //ATָ�������


#define  at_len   4

//#define  AT       0
//#define  DUANXIN  1
//#define  HTTP     2
//#define  MQTT     3
//�˽ṹ����Ϊ�˻ص����������ģ����ATָ��
typedef enum
{
  AT=0,
  DUANXIN,
  HTTP,
  MQTT, 
}send_at;//ѡ��ʹ���ĸ�AT��ģ��
typedef enum
{  ZERO=0,
  ONE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  TEN,
  QIACT_ONE,
  QIACT_TWO,
  QHTTPURL,
  HTTP_URL,
  QHTTPGET,
  QHTTPREAD,
}at_State;//����AT����ʱ��4G������һ��״̬��

typedef int (*at_call)(void *arg,void *ret);//����洢at��ָ��


typedef struct
{
  int select;
  at_call  at_callback;
}at_culi;



//ÿ�η�����ATָ��ͱ�־λ��һ
typedef struct 
{
  int at_flag;
  int duanxin_flag;
  int http_flag;
  int mqtt_flag;
}at_flag;



void EC800_receive_task(void const * argument);
void EC800_send_task(void const * argument);


bool sendATCommand(int index);//���ͺ���
bool checkResponse(int index);//У�麯��
int at_send_OK(send_at at );           //�ж�ATָ���Ƿ��ͳɹ�
int at_at_duanxin(void *arg,void *ret);//���Ͷ���
int at_at_at(void *arg,void *ret);//ȱ���Լ�
int at_at_http(void *arg,void *ret);// http,��ATָ��
int at_at_mqtt(void *arg,void *ret);//mqtt,��ATָ��


#endif