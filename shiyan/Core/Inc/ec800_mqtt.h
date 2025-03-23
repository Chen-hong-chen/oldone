#ifndef _EC800_MQTT_H_
#define _EC800_MQTT_H_
#include "stdbool.h"
bool mqtt_json_send();//组成json数据发送

bool mqtt_json_receive(); //接受json数据解析
void tianqi_json_get(void *arg,void *ret);
void gps_json_get(void *arg,void *ret);
#endif