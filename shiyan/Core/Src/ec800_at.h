#ifndef _EC800_AT_H_
#define _EC800_AT_H_
#include <stdbool.h>
#define AT_SIZE   256
#define  http_url    "https://restapi.amap.com/v3/assistant/coordinate/convert?locations=116.481499,39.990475&coordsys=gps&output=json&key=bfe9091507c1bc660bef259200c3e2d2"


#define  ST_COUNT  25   //AT指令集的数量


#define  at_len   4

//#define  AT       0
//#define  DUANXIN  1
//#define  HTTP     2
//#define  MQTT     3
//此结构体是为了回调函数，大规模调用AT指令
typedef enum
{
  AT=0,
  DUANXIN,
  HTTP,
  MQTT, 
}send_at;//选择使用哪个AT大模块
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
}at_State;//发送AT命令时，4G处于哪一个状态。

typedef int (*at_call)(void *arg,void *ret);//里面存储at的指令


typedef struct
{
  int select;
  at_call  at_callback;
}at_culi;



//每次发送完AT指令就标志位置一
typedef struct 
{
  int at_flag;
  int duanxin_flag;
  int http_flag;
  int mqtt_flag;
}at_flag;



void EC800_receive_task(void const * argument);
void EC800_send_task(void const * argument);


bool sendATCommand(int index);//发送函数
bool checkResponse(int index);//校验函数
int at_send_OK(send_at at );           //判断AT指令是否发送成功
int at_at_duanxin(void *arg,void *ret);//发送短信
int at_at_at(void *arg,void *ret);//缺乏自检
int at_at_http(void *arg,void *ret);// http,的AT指令
int at_at_mqtt(void *arg,void *ret);//mqtt,的AT指令


#endif