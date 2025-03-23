#ifndef _EC800_AT_H_
#define _EC800_AT_H_
#include <stdbool.h>
#define AT_send_SIZE   200
#define AT_rece_SIZE   50
#define  http_gps_len    "AT+QHTTPURL=149,80\r\n"
#define  http_url    "https://restapi.amap.com/v3/assistant/coordinate/convert?locations=%f,%f&coordsys=gps&output=json&key=bfe9091507c1bc660bef259200c3e2d2"

#define  http_tianqi_len   "AT+QHTTPURL=96,80\r\n"
#define  http_url_tianqi    "https://restapi.amap.com/v3/weather/weatherInfo?city=340104&key=bfe9091507c1bc660bef259200c3e2d2"

#define  error     "ERROR"
#define  ST_COUNT  30   //AT指令集的数量


#define  at_len   5

#define MAX_JSON_SIZE 320  //提取json数据

#define MAX__MQTT_JSON_SIZE 800
//#define  AT       0
//#define  DUANXIN  1
//#define  HTTP     2
//#define  MQTT     3
//此结构体是为了回调函数，大规模调用AT指令
typedef enum
{
  AT=0,
  DUANXIN,
  HTTPGPS,
  HTTPTIANQI,
  MQTT, 
}send_at;//选择使用哪个AT大模块
typedef enum
{  ZERO=0, //0
  ONE,     //1
  TWO,     //2     
  THREE,     //3
  FOUR,     //4
  FIVE,     //5
  SIX,     //6
  SEVEN,     //7
  EIGHT,     //8
  NINE,     //9
  TEN,     //10
  QIACT_ONE,     //11
  QIACT_TWO,     //12
  QHTTPURL,     //13
  HTTP_URL,     //14
  QHTTPGET,     //15
  QHTTPREAD,     //16
  
  QHTTPREADOK,     //17
  QMTCFGONE,       //18
  QMTCFGTWO,       //19       
  QMTOPEN,         //20
  QMTCONNONE,      //21
  QMTCONNTWO,      //22
  QMTPUBEX,        //23
  QMTDISC,         //24
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
  int http_fla_gps;
  int http_flag_tianqi;
  int mqtt_flag;
}at_at_flag1;



void EC800_receive_task(void const * argument);
void EC800_send_task(void const * argument);


bool sendATCommand(int index);//发送函数
bool checkResponse(int index);//校验函数
int at_send_OK(send_at at ,void *arg,void *ret);           //判断AT指令是否发送成功
int at_at_duanxin(void *arg,void *ret);//发送短信
int at_at_at(void *arg,void *ret);//缺乏自检
int at_at_http_gps(void *arg,void *ret);// GPS的AT指令
int at_at_http_tianqi(void *arg,void *ret);         //天气
int at_at_mqtt(void *arg,void *ret);//mqtt,的AT指令
void gps_json_get(void *arg,void *ret);
void tianqi_json_get(void *arg,void *ret);


#endif