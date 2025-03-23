#include "ec800_mqtt.h"
#include "ec800_at.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "pubdata.h"
#include "string.h"
#include <stdbool.h>
#include <cJSON.h>
#include <stdio.h>
#include <string.h>
#include "stdlib.h"


char tianqi_cam[FOUR][FOUR]=
{
  "晴", 
  "雨",
  "阴",
  "云"
};


extern EC80ReceiveType chuli;


//接收的
//{"method":"thing.service.property.set","id":"1904122364",
//      "params":{"LightSwitch":1,
//                "Cardguanli":1,
//                "Timemanage":1,
//                "Dingshi":1,
//                "dingshitimer":23,
//                },
//   "version":"1.0.0"
//  }"
//发送的
//    {"params":
//                {
//                  "GeoLocation":{"longitude":116.341727430556,"latitude":39.991803927952},
//                  "LightSwitch":1,
//                  "RunningState":0,
//                  "Cardguanli":1,
//                  "OverTiltError":0,
//                  "AlarmSwitch":0,
//                  "Timemanage":1,
//                  "Weather":0,
//                  "RSSI":25,
//                  "temperature":10
//                }
//    } 
char  str_mqtt[500]=NULL;
bool json_flag=false;
extern  at_at_flag1 at_send_flag;
bool mqtt_json_send()
{
    char*str_cjson;
    memset(str_mqtt,0,500);
    old_guai_zhang mqtt_buff_rece1;
    get_database(GPS_F,&mqtt_buff_rece1.gps_data);
    get_database(MPU_F,&mqtt_buff_rece1.mpu);
    get_database(CHAOSHENG_F,&mqtt_buff_rece1.chaosheng);
    get_database(LIGHT_F,&mqtt_buff_rece1.light);
    get_database(TIME_F,&mqtt_buff_rece1.time);
    get_database(XINHAO_T,&mqtt_buff_rece1.xinhao);
    get_database(AIR_F,&mqtt_buff_rece1.tianqi);
    get_database(SHUKA_F,&mqtt_buff_rece1.shuaka);
    get_database(DINGGSHI_TIMER,&mqtt_buff_rece1.dingshitimer);
    get_database(DINGSHI,&mqtt_buff_rece1.Dingshi);
    
    get_database(RUN,&mqtt_buff_rece1.run);

    cJSON*cjson_test = cJSON_CreateObject();
    
    if (cjson_test == NULL) {
      printf("cjson_test is error\r\n");
        return false; // 处理内存分配失败
    }
    cJSON*cjson_params = cJSON_CreateObject();
    if (cjson_test == NULL) {
      printf("cjson_params is error\r\n");
        return false; // 处理内存分配失败
    }
    cJSON_AddNumberToObject(cjson_params, "LightSwitch",mqtt_buff_rece1.light ); //OverTiltError
    cJSON_AddNumberToObject(cjson_params, "RunningState", mqtt_buff_rece1.run);
    cJSON_AddNumberToObject(cjson_params, "Cardguanli", mqtt_buff_rece1.shuaka);
    cJSON_AddNumberToObject(cjson_params, "Timemanage", mqtt_buff_rece1.time);
    cJSON_AddNumberToObject(cjson_params, "Weather", mqtt_buff_rece1.tianqi.tianqi1);
    cJSON_AddNumberToObject(cjson_params, "RSSI", mqtt_buff_rece1.xinhao);
    cJSON_AddNumberToObject(cjson_params, "temperature", mqtt_buff_rece1.tianqi.wendu);
    cJSON_AddNumberToObject(cjson_params, "Dingshi", mqtt_buff_rece1.Dingshi);
    cJSON_AddNumberToObject(cjson_params, "dingshitimer", mqtt_buff_rece1.dingshitimer);
    cJSON_AddNumberToObject(cjson_params, "OverTiltError", mqtt_buff_rece1.mpu);   //是否倾斜
    cJSON_AddNumberToObject(cjson_params, "apartpanduan", mqtt_buff_rece1.chaosheng);   //是否距离小于0.5米
    cJSON*cjson_GeoLocation = cJSON_CreateObject();
    cJSON_AddNumberToObject(cjson_GeoLocation, "longitude", mqtt_buff_rece1.gps_data.jingdu);
    cJSON_AddNumberToObject(cjson_GeoLocation, "latitude", mqtt_buff_rece1.gps_data.weidu);
    cJSON_AddItemToObject(cjson_params, "GeoLocation",cjson_GeoLocation);
    cJSON_AddItemToObject(cjson_test, "params", cjson_params);
    str_cjson=cJSON_Print(cjson_test);
    strcpy(str_mqtt,str_cjson);
    //printf("%s %d\n", str_mqtt,strlen(str_mqtt));
    free(str_cjson);
    cJSON_Delete(cjson_test);
    at_send_flag.mqtt_flag=false; 
    json_flag=true;    
  return true;
}

bool mqtt_json_receive()
{
        char mqtt_json_buff[MAX__MQTT_JSON_SIZE];//gps_json数据
        old_guai_zhang mqtt_buff_rece;
        char *start = strchr((const char *)chuli.usartDMA_rxBuf, '{'); // 找到第一个 '{'
        char *end = strrchr((const char *)chuli.usartDMA_rxBuf, '}');  // 找到最后一个 '}'
        //检查是否成功找到 JSON 部分
        if (start && end) 
        {
            //计算 JSON 部分的长度
            int length = end - start + 1;
            //检查长度是否超过最大限制
            if (length > MAX__MQTT_JSON_SIZE) {
                printf("JSON string is too long.\n");
            }
            //提取 JSON 部分
            strncpy(mqtt_json_buff, start, length);
            mqtt_json_buff[length] = '\0'; // 添加字符串结束符  
            printf("%s\n", mqtt_json_buff);
         }
      //json部分
       cJSON *json = cJSON_Parse(mqtt_json_buff);
      if (json == NULL) 
      {
        printf("JSON解析错误: %s\n", cJSON_GetErrorPtr());
        cJSON_Delete(json);
      }
      else
      {
             cJSON *params = cJSON_GetObjectItem(json, "params");
             if(params==NULL)
             {
               printf("mqtt params is error\r\n");
               return false;
             }
             else
             {
                cJSON *LightSwitch = cJSON_GetObjectItem(params, "LightSwitch");//light
                if (LightSwitch != NULL) 
                {
                      mqtt_buff_rece.light = LightSwitch->valueint; // 安全地访问 valueint
                      set_database(LIGHT_F, &mqtt_buff_rece.light); // 调用 set_database
                } 
                else 
                {
                       mqtt_buff_rece.light = 0; // 假设默认值为 0
                }
                cJSON *Cardguanli = cJSON_GetObjectItem(params, "Cardguanli");   //card
                if (Cardguanli != NULL) 
                {
                      mqtt_buff_rece.shuaka = Cardguanli->valueint;
                      set_database(SHUKA_F, &mqtt_buff_rece.shuaka);
                } 
                else 
                {
                      mqtt_buff_rece.shuaka = 0; // 设置默认值
                }
                cJSON *Timemanage = cJSON_GetObjectItem(params, "Timemanage");   //时间播报
                if (Timemanage != NULL) 
                {
                    mqtt_buff_rece.time = Timemanage->valueint;
                    set_database(TIME_F, &mqtt_buff_rece.time);
                }
                else 
                {
                    mqtt_buff_rece.time = 0; // 设置默认值
                }
                cJSON *Dingshi = cJSON_GetObjectItem(params, "Dingshi");        //开启定时 运动
                if (Dingshi != NULL) 
                {
                    mqtt_buff_rece.Dingshi = Dingshi->valueint;
                    set_database(DINGSHI, &mqtt_buff_rece.Dingshi);
                } 
                else 
                {
                    mqtt_buff_rece.Dingshi = 0; // 设置默认值
                }
                cJSON *dingshitimer1 = cJSON_GetObjectItem(params, "dingshitimer"); //提醒时间
                if (dingshitimer1!= NULL) 
                {
                    mqtt_buff_rece.dingshitimer = dingshitimer1->valueint;
                    set_database(DINGGSHI_TIMER, &mqtt_buff_rece.dingshitimer);
                } 
                else 
                {
                    mqtt_buff_rece.dingshitimer = 0; // 设置默认值
                }
             }
        cJSON_Delete(json);
      }
  return true;
}
void gps_json_get(void *arg,void *ret)
{       
        char gps_json_buff[MAX_JSON_SIZE];//gps_json数据
        char *pc;//gps_buf
        gps gps_json;
         
        char *start = strchr((const char *)chuli.usartDMA_rxBuf, '{'); // 找到第一个 '{'
        char *end = strrchr((const char *)chuli.usartDMA_rxBuf, '}');  // 找到最后一个 '}'
        //检查是否成功找到 JSON 部分
        if (start && end) 
        {
            //计算 JSON 部分的长度
            int length = end - start + 1;
            //检查长度是否超过最大限制
            if (length > MAX_JSON_SIZE) {
                printf("JSON string is too long.\n");
            }
            //提取 JSON 部分
            strncpy(gps_json_buff, start, length);
            gps_json_buff[length] = '\0'; // 添加字符串结束符  
            //printf("%s\n", gps_json_buff);
         }
      //json部分
      cJSON *json = cJSON_Parse(gps_json_buff);
      if (json == NULL) 
      {
        printf("JSON解析错误: %s\n", cJSON_GetErrorPtr());
        cJSON_Delete(json);
      }
      else
      {
          cJSON *status = cJSON_GetObjectItem(json, "status");
           if(strncmp(status->valuestring,"1",1) == 0)
           {
             cJSON *locations = cJSON_GetObjectItem(json, "locations");
              pc = strtok(locations->valuestring,","); 
              //116.487585177952
              gps_json.jingdu = atof(pc);
              pc = strtok(NULL,"\0");       
              //39.991754014757
              gps_json.weidu = atof(pc);
              set_database(GPS_F,&gps_json);
              //printf("jingdu = %.12f\r\nweidu = %.12f\r\n",gps_json.jingdu,gps_json.weidu);
           }
           else
           {
             cJSON_Delete(json);
           }
        cJSON_Delete(json);
       }
}
void tianqi_json_get(void *arg,void *ret)
{
      //char *pc;//gps_buf
      char tianqi_json_buff[MAX_JSON_SIZE];//tianqi_json数据
      char tianqi_json_buff2[MAX_JSON_SIZE];//tianqi_json数据
      tianqi_wendu tianqi_buff;
      char *start1 = strchr((const char *)chuli.usartDMA_rxBuf, '{'); // 找到第一个 '{'
      char *end1 = strrchr((const char *)chuli.usartDMA_rxBuf, '}');  // 找到最后一个 '}'
      //检查是否成功找到 JSON 部分
       if (start1 && end1) 
       {
          //计算 JSON 部分的长度
          int length = end1 - start1 + 1;
          //检查长度是否超过最大限制
          if (length > MAX_JSON_SIZE) 
          {
             printf("JSON string is too long.\n");
          }
          //提取 JSON 部分
          char json_str1[MAX_JSON_SIZE]; // +1 用于存储字符串结束符 '\0'
          strncpy(json_str1, start1, length);
          json_str1[length] = '\0'; // 添加字符串结束符
          strncpy(tianqi_json_buff,&json_str1[1],length-2);
          //tianqi_json_buff2
          char *start2 = strchr((const char *)tianqi_json_buff, '{'); // 找到第一个 '{'
          char *end2 = strrchr((const char *)tianqi_json_buff, '}');  // 找到最后一个 '}'
           if (start2 && end2) 
            {
               int length1 = end2 - start2 + 1;
               strncpy(tianqi_json_buff2, start2, length1);
               tianqi_json_buff2[length1] = '\0';
            }
          //输出结果
          ///printf("%s\n", tianqi_json_buff2);
        }
      cJSON *json1 = cJSON_Parse(tianqi_json_buff2);
      if (json1 == NULL) 
      {
        printf("JSON解析错误: %s\n", cJSON_GetErrorPtr());
        cJSON_Delete(json1);
      }
      else
      {
          cJSON *weather = cJSON_GetObjectItem(json1, "weather");
          //printf("tianqi = %s",weather->valuestring);
            if(strstr(weather->valuestring,tianqi_cam[ZERO])!=NULL)
              {
                tianqi_buff.tianqi1=0;
              }
              else if(strstr(weather->valuestring,tianqi_cam[ONE])!=NULL)
              {
                tianqi_buff.tianqi1=1;
              }
              else if(strstr(weather->valuestring,tianqi_cam[THREE])!=NULL)
              {
                tianqi_buff.tianqi1=3;
              }
              else
              {
                tianqi_buff.tianqi1=2;
              }
              cJSON *temperature = cJSON_GetObjectItem(json1, "temperature");
              tianqi_buff.wendu= atoi(temperature->valuestring);
              set_database(AIR_F,&tianqi_buff);
              //printf("tianqi = %d\r\nwendu = %d\r\n",tianqi_buff.tianqi1,tianqi_buff.wendu);
              cJSON_Delete(json1);
     }   
}
