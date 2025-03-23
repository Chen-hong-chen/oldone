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
#include "ec800_mqtt.h"
extern osTimerId sendTimerHandle;
extern bool json_flag;//json已经组好包
extern char  str_mqtt[500]; //存储json的组包的数据
extern gps  gps_buff;//gps的结构体
extern old_guai_zhang oldone;//拐杖的数据库
at_at_flag1 at_send_flag;//每发送一次AT模块标志位都置一
send_at at_select;//选择使用哪个AT大模块
char rx_buffer[EC80_RECEIVE_LEN];//缓存数组关于4G
extern UART_HandleTypeDef huart4;
extern osMessageQId EC800_4G_ReceQueueHandle;//创建4G的消息队列
//回调函数使能AT指令
at_culi at_solve[at_len]=
{
  {AT,at_at_at},
  {DUANXIN,at_at_duanxin},
  {HTTPGPS,at_at_http_gps},
  {HTTPTIANQI,at_at_http_tianqi},
  {MQTT,at_at_mqtt}
};

char AT_commend_send[ST_COUNT][AT_send_SIZE] =
{//one
    "AT\r\n",                        //0 at检测       
    "ATE0\r\n",                       //1 关闭回显                                          
    "AT+CPIN?\r\n",                  //2 检查卡是否有PIN码  
      
  //two    
    "AT+CSMP=17,167,0,8\r\n",         // 3  发送中文短信固定格式
    "AT+CMGF=1\r\n",                  //4   消息模式文本
    "AT+CSCS=\"GSM\"\r\n",            //5 字符集格式为 GSM
    
    
    "AT+CMGS=\"19356512087\"\r\n",    // 6 发送手机号
    "62d0675650124e86",              //7 发送中文短信(电车倒了)
    {0x1A},                            //8 可以发送
    
    //three
    "AT+QHTTPCFG=\"contextid\",1\r\n",    //9 配置上下文
    "AT+QICSGP=1,1,\"CMNET\","","",1\r\n",  // 10配置场景PDP
    "AT+QIACT=1\r\n",                     //11 激活场景PDP
    "AT+QIACT?\r\n",                     //12  检查PDP 
    
    //htpp
    "AT+QHTTPURL=149,80\r\n",            //13 设置URL（坐标的）
                            
     http_url,                           //14 写入URL获取坐标
    "AT+QHTTPGET=80\r\n",               //15获取HTTP信息
    "AT+QHTTPREAD=80\r\n",                   //16读取信息
    
    //mqtt
    "AT+QMTCFG=\"recv/mode\",0,0,1\r\n",   //17QMTCFG 配置接收模式
    "AT+QMTCFG=\"qmtping\",0,60\r\n",      //18配置心跳  QMTCFG
    "AT+QMTCFG=\"aliauth\",0,\"k1wmoWJ6QzC\",\"oneold\",\"d165a99a4059fb40b24c59e142fbd0c0\"\r\n", 
                                           //19配置阿里云设备信息三元组
    "AT+QMTOPEN=0,\"iot-06z00j8ljrlxenw.mqtt.iothub.aliyuncs.com\",1883\r\n",
                                           //20MQTT 客户端打开网络
    "AT+QMTCONN=0,\"oneold\"\r\n",         //21客户端连接 MQTT 服务器
    "AT+QMTCONN?\r\n",                    //22判断客户端连接 MQTT 服务器
    "AT+QMTPUBEX=0,0,0,1,\"/sys/k1wmoWJ6QzC/oneold/thing/event/property/post\",500\r\n",
                                      //23发布主题
    "AT+QMTDISC=0\r\n",               //24断开客户端在 MQTT 服务器的连接
    
    "AT+CSQ\r\n",                        // 显示信号强度
    "AT+QNTP=1,\"ntp.aliyun.com\"\r\n"   // 显示时间
};
static char AT_commend_receive[ST_COUNT][AT_rece_SIZE] =
{
    "OK",                        //0 at检测       
    "OK",                       //1 关闭回显                                          
    "READY",                  //2 检查卡是否有PIN码  
      
      
    "OK",         // 3  发送中文短信固定格式
    "OK",                  //4   消息模式文本
    "OK",            //5 字符集格式为 GSM
    {0x0D,0x0A,0x3E,0x20 },    // 6 发送手机号
    "",              //7 发送中文短信(电车倒了)
    "+CMGS:42\r\nOK\r\n",                            //8 可以发送
    "OK",                //9 NULL
    "OK",               //10 NULL
    "",                //11 NULL
    "OK",               //12
      
    "CONNECT",        //13QHTTPURL
    "OK",               //14URL
    "200",              //15QHTTPGET
    "",               //16
    //mqtt
    "OK",                //17 
    "OK",                //18 
    "OK",                //19
    "OK",                //20
    "OK",                //21
    "QMTCONN: 0,3",                //22
    {0x3e},                //23 用来接受是否发送了正确的信息
};



EC80ReceiveType chuli={.rx_len=0,.receive_flag=0,.usartDMA_rxBuf={0}};//把接受的数据传入到发送函数中

void EC800_receive_task(void const * argument)
{
  
 /* USER CODE BEGIN Usart_4GTask */
  __HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE); //使能空闲中断
  HAL_UART_Receive_DMA(&huart4,(uint8_t * )rx_buffer,EC80_RECEIVE_LEN); //开启DMA接收
  /* Infinite loop */

  EC80ReceiveType p={.rx_len=0,.receive_flag=0,.usartDMA_rxBuf={0}};


  for(;;)
  {
    xQueueReceive(EC800_4G_ReceQueueHandle,(void *)&p,portMAX_DELAY);
    memset(chuli.usartDMA_rxBuf,0,EC80_RECEIVE_LEN);
    memcpy(chuli.usartDMA_rxBuf,p.usartDMA_rxBuf,p.rx_len);
    memcpy(&chuli.rx_len,&p.rx_len,sizeof(p.rx_len));
   if(strstr((const char *)chuli.usartDMA_rxBuf, "locations") != NULL)
    { 
       gps_json_get(NULL,NULL);
     } 
   if(strstr((const char *)chuli.usartDMA_rxBuf, "weather") != NULL)
    {  
      tianqi_json_get(NULL,NULL);
    }
   if((strstr((const char *)chuli.usartDMA_rxBuf, "QMTRECV") != NULL)&&(strstr((const char *)chuli.usartDMA_rxBuf, "params") != NULL))
   {
     if(mqtt_json_receive()==true)
     {
        printf("接收数据\r\n");
     }
   }
   // printf("%s\r\n",chuli.usartDMA_rxBuf);
    
    osDelay(10);
  
  }
  /* USER CODE END Usart_4GTask */
}


void EC800_send_task(void const * argument)
{ 
  for(;;)
  {
      
      if(strstr((const char *)chuli.usartDMA_rxBuf,"RDY")!=NULL) //起始函数
      {
      osDelay(1000);
      at_send_OK(AT,NULL,NULL);
      at_send_OK(HTTPTIANQI,NULL,NULL);
      osTimerStart(sendTimerHandle, 1000);//开启定时器 开启MQTT
      }
      osDelay(2000);
  }
  /* USER CODE END Usart_4GTask */
}

int at_at_mqtt(void *arg,void *ret)
{
 static int  flag_mqtt=QMTCONNTWO;
 int  flag_mqtt_a=0;
 while( at_send_flag.mqtt_flag==false&&at_send_flag.at_flag==true)
 {
 switch(flag_mqtt)
    {
     case QMTOPEN:  //20
           if (!sendATCommand(flag_mqtt)) {
                return 0; // 发送失败，退出
            }
           osDelay(500);
           flag_mqtt=QMTCONNONE;
      break;
    case QMTCONNONE:  //21
           if (!sendATCommand(flag_mqtt)) {
                return 0; // 发送失败，退出
            }
           osDelay(500);
           flag_mqtt=QMTCONNTWO;
      break;
    case QMTCONNTWO://23
           if (!sendATCommand(flag_mqtt)) {
                return 0; // 发送失败，退出
            }
           osDelay(1000);
           
           if (strstr((const char *)chuli.usartDMA_rxBuf, AT_commend_receive[QMTCONNTWO]) == NULL) 
            {
               flag_mqtt=QMTOPEN;
               at_send_flag.mqtt_flag=false;  
                break; // 响应不匹配，继续等待
            } 
           if(json_flag==false)
           {
           flag_mqtt=QMTDISC;
           }
           else
           {
             flag_mqtt=QMTPUBEX;
           }
      break;
    case QMTPUBEX:
         if (!sendATCommand(flag_mqtt)) {
                return 0; // 发送失败，退出
            }
         if(strstr((const char *)chuli.usartDMA_rxBuf, AT_commend_receive[QMTPUBEX]) != NULL)
         {
           HAL_UART_Transmit(EC80_UART,(const uint8_t *)str_mqtt, sizeof(str_mqtt), (u32)0xFFFF);
           
         }
          flag_mqtt=QMTPUBEX;
          at_send_flag.mqtt_flag=true;         
      break;
    case QMTDISC:
          at_send_flag.mqtt_flag=true;
          flag_mqtt=QMTCONNTWO;
      break;                          
    default:
      
       break;
       
    }
     flag_mqtt_a++;
   if(flag_mqtt_a>20)
       {
         flag_mqtt_a=QMTOPEN;
         flag_mqtt=0;
         return 0;
       }
 }
  return 0;
}
int at_at_at(void *arg,void *ret)
{

 static  int  flag1=0;//标志位
 static int  at_a=0;//自检
while(at_send_flag.at_flag==false)
{
   switch(flag1)
      { 
       case ZERO:
           if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1=ONE;
         break;
      case ONE:
           if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1=TWO;
         break;
     case TWO:
          if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1=THREE;

         break;
     case THREE:
          if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
        flag1=FOUR;
         break;
     case FOUR:
          if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
        flag1=FIVE;
         break;
     case FIVE:
          if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1=NINE;
         break;
     case NINE:
          if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
          }
         flag1=TEN;
         break;
     case TEN:
       if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
        if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
        flag1=QIACT_ONE;
         break;
     case QIACT_ONE:
         if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
           
         flag1=QIACT_TWO;
         break;
     case QIACT_TWO:
           if (!sendATCommand(flag1)) {
                          return 0; // 发送失败，退出
                      }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }

         flag1=QHTTPREADOK;
         break;
       case QHTTPREADOK:   //17
           if (!sendATCommand(flag1)) {
                          return 0; // 发送失败，退出
                      }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }

         flag1=QMTCFGONE;    
         break;
     case QMTCFGONE:   //18
           if (!sendATCommand(flag1)) {
                          return 0; // 发送失败，退出
                      }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }

         flag1=QMTCFGTWO;    
         break;
     case QMTCFGTWO:   //19
           if (!sendATCommand(flag1)) {
                          return 0; // 发送失败，退出
                      }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }

         flag1=QMTOPEN;    
         break;
     case QMTOPEN:  //20
           if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
           osDelay(1000);
           flag1=QMTCONNONE;
      break;
    case QMTCONNONE:  //21
           if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
           osDelay(500);
           flag1=QMTCONNTWO;
      break;
    case QMTCONNTWO://23
           if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
           osDelay(300);    //为了接受第二个回答。

           
           flag1=QMTDISC;
      break;
      case QMTDISC:
         flag1=0;
         at_a=0;
         at_send_flag.at_flag=true;
        break;
      default:
         break;
      }
      at_a++;
   if(at_a>30)
       {
         at_a=0;
         flag1=0;
         return 0;
       }
}
  return 0;
}
int at_at_duanxin(void *arg,void *ret)
{
 static int  flag=SIX;//标志位
 static int  duanxin_a=0;//自检
while(at_send_flag.duanxin_flag==true&&at_send_flag.at_flag==true)
 {
 switch(flag)
    { 
   case SIX:
        if (!sendATCommand(flag)) 
       {
           return 0; // 发送失败，退出
       }
       break;
   case SEVEN:
       if (!sendATCommand(flag)) 
       {
           return 0; // 发送失败，退出
       }
       flag+=1;
       break;
   case EIGHT:
       if (!sendATCommand(flag)) 
       {
            return 0; // 发送失败，退出
       }
       if (!checkResponse(flag)) 
       {
            break; // 响应不匹配，继续等待
        }
       flag+=1;
       break;
    case NINE:
        flag=6;
        duanxin_a=0;
        at_send_flag.duanxin_flag=false;
        break;
    default:
      
       break;
    }
  if(duanxin_a++>20)
       {
         duanxin_a=0;
         flag=0;
         return 0;
       }
 }
  return 0;
}
int at_at_http_tianqi(void *arg,void *ret)
{
 static int  flag_http_tianqi=QHTTPURL;
 static int  http_a_tianqi=ZE;//自检
 while(at_send_flag.http_flag_tianqi==false&&at_send_flag.at_flag==true)
 {
 switch(flag_http_tianqi)
    { 

      case QHTTPURL:
     HAL_UART_Transmit(EC80_UART,http_tianqi_len,strlen(http_tianqi_len), (u32)0xFFFF);
         osDelay(100);  
         flag_http_tianqi=HTTP_URL;
         break;
    case HTTP_URL:
     HAL_UART_Transmit(EC80_UART,http_url_tianqi, strlen(http_url_tianqi), (u32)0xFFFF);
           osDelay(50);
        flag_http_tianqi=QHTTPGET;
         break;  
    case QHTTPGET:
      sendATCommand(flag_http_tianqi);
        osDelay(500);
      flag_http_tianqi=QHTTPREAD;        
         break;           
    case QHTTPREAD:
       if (!sendATCommand(flag_http_tianqi)) 
       {
             return 0; // 发送失败，退出
       }
       if(strstr((const char *)chuli.usartDMA_rxBuf,error) != NULL)
       {
         flag_http_tianqi=QHTTPURL;
         http_a_tianqi=0;
        at_send_flag.http_flag_tianqi=false;
        osDelay(3000);
          break;
       }
         flag_http_tianqi=QHTTPREADOK;
         break; 
    case QHTTPREADOK:
        flag_http_tianqi=QHTTPURL;
         http_a_tianqi=0;
        at_send_flag.http_flag_tianqi=true;
        break;
    default:
      
       break;
       
    }
   if(http_a_tianqi++>30)
       {
         http_a_tianqi=ZE;
         flag_http_tianqi=QHTTPURL;
         return 0;
       }
 }
  return 0;
}
  int a=0;
int at_at_http_gps(void *arg,void *ret)
{

 static int  flag_http_gps=QHTTPURL;
 static int  http_a=ZE;//自检
 
 while(at_send_flag.http_fla_gps==false)
 {
 switch(flag_http_gps)
    { 
     case QHTTPURL:
       //printf("%s\r\n",AT_commend_send[QHTTPURL]);
     HAL_UART_Transmit(EC80_UART,(uint8_t const*)AT_commend_send[QHTTPURL], strlen(AT_commend_send[QHTTPURL]), (u32)0xFFFF);
         osDelay(100);
         flag_http_gps=HTTP_URL;
         break;
    case HTTP_URL:
       //printf("%s\r\n",(uint8_t const*)AT_commend_send[HTTP_URL]);
     HAL_UART_Transmit(EC80_UART,(uint8_t const*)AT_commend_send[HTTP_URL], strlen(AT_commend_send[HTTP_URL]), (u32)0xFFFF);
        flag_http_gps=QHTTPGET;
       osDelay(50);
         break;  
    case QHTTPGET:
      sendATCommand(flag_http_gps);
        osDelay(800);
       flag_http_gps=QHTTPREAD;        
         break;           
    case QHTTPREAD:
       if (!sendATCommand(flag_http_gps)) {
                          return 0; // 发送失败，退出
                      }
        if(strstr((const char *)chuli.usartDMA_rxBuf,error) != NULL)
       {
         flag_http_gps=QHTTPURL;
         http_a=0;
         at_send_flag.http_fla_gps=false;
         osDelay(3000);
         break; 
       }
         flag_http_gps=QHTTPREADOK;
         break; 
    case QHTTPREADOK:
        flag_http_gps=0;
         http_a=0;
        at_send_flag.http_fla_gps=true;
        break;
    default:
      
       break;
       
    }
   if(http_a++>20)
       {
         http_a=ZE;
         flag_http_gps=QHTTPURL;
         return 0;
       }
 }
  return 0;
}
//mqtt


int at_send_OK(send_at at,void *arg,void *ret)
{

  for(int i=ZE;i<at_len;i++)
    {
      
       if(at_solve[i].select==at)
      {
         at_solve[i].at_callback(arg,ret);
       }
    }
    
  return 0;
  
}
bool sendATCommand(int index) {
   // printf("%s\r\n",AT_commend_send[index]);
    if (HAL_UART_Transmit(EC80_UART, (const uint8_t *)AT_commend_send[index], sizeof(AT_commend_send[index]), (u32)0xFFFF) != HAL_OK) {
        return false; //发送失败
    }
    osDelay(50);
    return true;
}

bool checkResponse(int index) {
    return strstr((const char *)chuli.usartDMA_rxBuf, AT_commend_receive[index]) != NULL;
    
}


