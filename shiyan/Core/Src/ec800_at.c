#include "ec800_at.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "pubdata.h"
#include "string.h"
#include <stdbool.h>



extern gps  gps_buff;//gps的结构体
extern old_guai_zhang oldone;//拐杖的数据库
at_flag at_send_flag;//每发送一次AT模块标志位都置一
send_at at_select;//选择使用哪个AT大模块
char rx_buffer[EC80_RECEIVE_LEN];//缓存数组关于4G
extern UART_HandleTypeDef huart4;
extern osMessageQId EC800_4G_ReceQueueHandle;//创建4G的消息队列
//回调函数使能AT指令
at_culi at_solve[at_len]=
{
  {AT,at_at_at},
  {DUANXIN,at_at_duanxin},
  {HTTP,at_at_http},
  {MQTT,at_at_mqtt}
};

char AT_commend_send[ST_COUNT][AT_SIZE] =
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
    
    
    
    "AT+QHTTPURL=149,80\r\n",            //13 设置URL（坐标的）
  
                                          
     http_url,                           //14 写入URL获取坐标
    "AT+QHTTPGET =80\r\n",               //15获取HTTP信息
    "AT+QHTTPREAD =80",                   //16读取信息
    
    
    "AT+CSQ\r\n",                        // 显示信号强度
    "AT+QNTP=1,\"ntp.aliyun.com\"\r\n"   // 显示时间
};
char AT_commend_receive[ST_COUNT][50] =
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
      
    "CONNECT",               //13
    "OK",               //14
    "",              //15
    "",               //16
    ""                //17 
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
    memcpy(chuli.usartDMA_rxBuf,p.usartDMA_rxBuf,p.rx_len);
   
    osDelay(10);
  }
  /* USER CODE END Usart_4GTask */
}


void EC800_send_task(void const * argument)
{
 
  memset(&at_send_flag,false,sizeof(at_send_flag));
 
   
  for(;;)
  {
      at_send_OK(AT);
      at_send_OK(HTTP);
//   if(at_send_flag.at_flag==false)
//   {
//     
//          at_send_OK(AT);
//
//        at_send_flag.http_flag=true;
//        at_send_flag.mqtt_flag=true;
//
//  
//   }
//   else
//   {
//       if(at_send_flag.duanxin_flag==true)
//       {
//         at_send_OK(DUANXIN);
//        }
////       if(at_send_flag.http_flag==true&&oldone2.jingdu!=0&&oldone2.weidu!=0&&at_send_flag.duanxin_flag==false)
////       {
////         //at_send_OK(HTTP);
////        }
//        printf("this is no ok\r\n");
//     
//   }
      osDelay(5000);
  }
  /* USER CODE END Usart_4GTask */
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
         flag1+=1;
         break;
      case ONE:
           if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1+=1;

         break;
     case TWO:
          if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1+=1;

         break;
     case THREE:
          if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1+=1;
         break;
     case FOUR:
          if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1+=1;
         break;
     case FIVE:
          if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1+=4;
         break;
     case SIX:
          if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
          }
           if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1+=1;
         break;
     case TEN:
       if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
        if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1+=1;
         break;
     case QIACT_ONE:
         if (!sendATCommand(flag1)) {
                return 0; // 发送失败，退出
            }
           
         flag1+=1;
         break;
     case QIACT_TWO:
           if (!sendATCommand(flag1)) {
                          return 0; // 发送失败，退出
                      }
            if (!checkResponse(flag1)) {
                break; // 响应不匹配，继续等待
            }
         flag1+=1;
         break;
      case 13:
         flag1=0;
         at_a=0;
         at_send_flag.at_flag=true;
        break;
      default:
         break;
      }
      at_a++;
   if(at_a>20)
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
 static int  flag=6;//标志位
 static int  duanxin_a=0;//自检
while(at_send_flag.duanxin_flag==true)
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
int at_at_http(void *arg,void *ret)
{
 static int  flag_http=13;
 while(oldone.gps_data.jingdu!=0&&oldone.gps_data.weidu!=0)
 {
 switch(flag_http)
    { 
    case QHTTPURL:
       if (!sendATCommand(flag_http)) {
                          return 0; // 发送失败，退出
                      }
       if (!checkResponse(flag_http)) {
                break; // 响应不匹配，继续等待
            }
         flag_http+=1;
         break;
    case HTTP_URL:
       memset(AT_commend_send[HTTP_URL],0,AT_SIZE);
       sprintf(AT_commend_send[HTTP_URL],http_url,gps_buff.jingdu,gps_buff.weidu);
       if (!sendATCommand(flag_http)) {
                          return 0; // 发送失败，退出
                      }
       if (!checkResponse(flag_http)) {
                break; // 响应不匹配，继续等待
            }
         flag_http+=1;
         break;  
    case QHTTPGET:
     
       if (!sendATCommand(flag_http)) {
                          return 0; // 发送失败，退出
                      }
       if (!checkResponse(flag_http)) {
               break; // 响应不匹配，继续等待
            }
        flag_http+=1;
         break;           
    case QHTTPREAD:
     
       if (!sendATCommand(flag_http)) {
                          return 0; // 发送失败，退出
                      }
       if (!checkResponse(flag_http)) {
                break; // 响应不匹配，继续等待
            }
         flag_http+=1;
         break;                    
    default:
      
       break;
       
    }
 }
  return 0;
}

int at_at_mqtt(void *arg,void *ret)
{
 static int  flag_mqtt=0;
 switch(flag_mqtt)
    { 
    case 10:
                  
      return flag_mqtt;
    default:
      
       break;
       
    }
  
  
  return 0;
}

int at_send_OK(send_at at )
{

  for(int i=0;i<4;i++)
    {
      
       if(at_solve[i].select==at)
      {
        
         at_solve[i].at_callback(NULL,NULL);
       }
    }
    
  return 0;
  
}
bool sendATCommand(int index) {
    if (HAL_UART_Transmit(EC80_UART, (const uint8_t *)AT_commend_send[index], sizeof(AT_commend_send[index]), (u32)0xFFFF) != HAL_OK) {
        return false; // 发送失败
    }
    osDelay(50);
    return true;
}

bool checkResponse(int index) {
    return strstr((const char *)chuli.usartDMA_rxBuf, AT_commend_receive[index]) != NULL;
}

