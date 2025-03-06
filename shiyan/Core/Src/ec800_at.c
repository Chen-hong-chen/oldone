#include "ec800_at.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "pubdata.h"
#include "string.h"
#include <stdbool.h>



extern gps  gps_buff;//gps�Ľṹ��
extern old_guai_zhang oldone;//���ȵ����ݿ�
at_flag at_send_flag;//ÿ����һ��ATģ���־λ����һ
send_at at_select;//ѡ��ʹ���ĸ�AT��ģ��
char rx_buffer[EC80_RECEIVE_LEN];//�����������4G
extern UART_HandleTypeDef huart4;
extern osMessageQId EC800_4G_ReceQueueHandle;//����4G����Ϣ����
//�ص�����ʹ��ATָ��
at_culi at_solve[at_len]=
{
  {AT,at_at_at},
  {DUANXIN,at_at_duanxin},
  {HTTP,at_at_http},
  {MQTT,at_at_mqtt}
};

char AT_commend_send[ST_COUNT][AT_SIZE] =
{//one
    "AT\r\n",                        //0 at���       
    "ATE0\r\n",                       //1 �رջ���                                          
    "AT+CPIN?\r\n",                  //2 ��鿨�Ƿ���PIN��  
      
  //two    
    "AT+CSMP=17,167,0,8\r\n",         // 3  �������Ķ��Ź̶���ʽ
    "AT+CMGF=1\r\n",                  //4   ��Ϣģʽ�ı�
    "AT+CSCS=\"GSM\"\r\n",            //5 �ַ�����ʽΪ GSM
    
    
    "AT+CMGS=\"19356512087\"\r\n",    // 6 �����ֻ���
    "62d0675650124e86",              //7 �������Ķ���(�糵����)
    {0x1A},                            //8 ���Է���
    
    //three
    "AT+QHTTPCFG=\"contextid\",1\r\n",    //9 ����������
    "AT+QICSGP=1,1,\"CMNET\","","",1\r\n",  // 10���ó���PDP
    "AT+QIACT=1\r\n",                     //11 �����PDP
    "AT+QIACT?\r\n",                     //12  ���PDP 
    
    
    
    "AT+QHTTPURL=149,80\r\n",            //13 ����URL������ģ�
  
                                          
     http_url,                           //14 д��URL��ȡ����
    "AT+QHTTPGET =80\r\n",               //15��ȡHTTP��Ϣ
    "AT+QHTTPREAD =80",                   //16��ȡ��Ϣ
    
    
    "AT+CSQ\r\n",                        // ��ʾ�ź�ǿ��
    "AT+QNTP=1,\"ntp.aliyun.com\"\r\n"   // ��ʾʱ��
};
char AT_commend_receive[ST_COUNT][50] =
{
    "OK",                        //0 at���       
    "OK",                       //1 �رջ���                                          
    "READY",                  //2 ��鿨�Ƿ���PIN��  
      
      
    "OK",         // 3  �������Ķ��Ź̶���ʽ
    "OK",                  //4   ��Ϣģʽ�ı�
    "OK",            //5 �ַ�����ʽΪ GSM
    {0x0D,0x0A,0x3E,0x20 },    // 6 �����ֻ���
    "",              //7 �������Ķ���(�糵����)
    "+CMGS:42\r\nOK\r\n",                            //8 ���Է���
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


EC80ReceiveType chuli={.rx_len=0,.receive_flag=0,.usartDMA_rxBuf={0}};//�ѽ��ܵ����ݴ��뵽���ͺ�����

void EC800_receive_task(void const * argument)
{
  
 /* USER CODE BEGIN Usart_4GTask */
  __HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE); //ʹ�ܿ����ж�
  HAL_UART_Receive_DMA(&huart4,(uint8_t * )rx_buffer,EC80_RECEIVE_LEN); //����DMA����
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

 static  int  flag1=0;//��־λ
 static int  at_a=0;//�Լ�
while(at_send_flag.at_flag==false)
{
   switch(flag1)
      { 
       case ZERO:
           if (!sendATCommand(flag1)) {
                return 0; // ����ʧ�ܣ��˳�
            }
            if (!checkResponse(flag1)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
         flag1+=1;
         break;
      case ONE:
           if (!sendATCommand(flag1)) {
                return 0; // ����ʧ�ܣ��˳�
            }
            if (!checkResponse(flag1)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
         flag1+=1;

         break;
     case TWO:
          if (!sendATCommand(flag1)) {
                return 0; // ����ʧ�ܣ��˳�
            }
            if (!checkResponse(flag1)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
         flag1+=1;

         break;
     case THREE:
          if (!sendATCommand(flag1)) {
                return 0; // ����ʧ�ܣ��˳�
            }
            if (!checkResponse(flag1)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
         flag1+=1;
         break;
     case FOUR:
          if (!sendATCommand(flag1)) {
                return 0; // ����ʧ�ܣ��˳�
            }
            if (!checkResponse(flag1)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
         flag1+=1;
         break;
     case FIVE:
          if (!sendATCommand(flag1)) {
                return 0; // ����ʧ�ܣ��˳�
            }
            if (!checkResponse(flag1)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
         flag1+=4;
         break;
     case SIX:
          if (!sendATCommand(flag1)) {
                return 0; // ����ʧ�ܣ��˳�
          }
           if (!checkResponse(flag1)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
         flag1+=1;
         break;
     case TEN:
       if (!sendATCommand(flag1)) {
                return 0; // ����ʧ�ܣ��˳�
            }
        if (!checkResponse(flag1)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
         flag1+=1;
         break;
     case QIACT_ONE:
         if (!sendATCommand(flag1)) {
                return 0; // ����ʧ�ܣ��˳�
            }
           
         flag1+=1;
         break;
     case QIACT_TWO:
           if (!sendATCommand(flag1)) {
                          return 0; // ����ʧ�ܣ��˳�
                      }
            if (!checkResponse(flag1)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
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
 static int  flag=6;//��־λ
 static int  duanxin_a=0;//�Լ�
while(at_send_flag.duanxin_flag==true)
 {
 switch(flag)
    { 
   case SIX:
        if (!sendATCommand(flag)) 
       {
           return 0; // ����ʧ�ܣ��˳�
       }
       break;
   case SEVEN:
       if (!sendATCommand(flag)) 
       {
           return 0; // ����ʧ�ܣ��˳�
       }
       flag+=1;
       break;
   case EIGHT:
       if (!sendATCommand(flag)) 
       {
                return 0; // ����ʧ�ܣ��˳�
        }
       if (!checkResponse(flag)) 
       {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
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
                          return 0; // ����ʧ�ܣ��˳�
                      }
       if (!checkResponse(flag_http)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
         flag_http+=1;
         break;
    case HTTP_URL:
       memset(AT_commend_send[HTTP_URL],0,AT_SIZE);
       sprintf(AT_commend_send[HTTP_URL],http_url,gps_buff.jingdu,gps_buff.weidu);
       if (!sendATCommand(flag_http)) {
                          return 0; // ����ʧ�ܣ��˳�
                      }
       if (!checkResponse(flag_http)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
         flag_http+=1;
         break;  
    case QHTTPGET:
     
       if (!sendATCommand(flag_http)) {
                          return 0; // ����ʧ�ܣ��˳�
                      }
       if (!checkResponse(flag_http)) {
               break; // ��Ӧ��ƥ�䣬�����ȴ�
            }
        flag_http+=1;
         break;           
    case QHTTPREAD:
     
       if (!sendATCommand(flag_http)) {
                          return 0; // ����ʧ�ܣ��˳�
                      }
       if (!checkResponse(flag_http)) {
                break; // ��Ӧ��ƥ�䣬�����ȴ�
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
        return false; // ����ʧ��
    }
    osDelay(50);
    return true;
}

bool checkResponse(int index) {
    return strstr((const char *)chuli.usartDMA_rxBuf, AT_commend_receive[index]) != NULL;
}

