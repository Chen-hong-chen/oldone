#include "chaochengbo.h"
#include "main.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "usart.h"
#include "usart.h"
#include "pubdata.h"
uint32_t chashengbo_flag1=0;
double julia=0;
double cha=0;
uint32_t cs_flag1=0;//��־λ
uint32_t IC_VALUE2=0;//ʱ��ֵ
uint32_t IC_VALUE1=0;//ʱ��ֵ
float juli(int qian,int hou)
{
	

	if(hou>qian)
	{
            cha=hou-qian;
	    cha  = cha*340/2/1000000; 
             return  cha;
	} 
         else if(qian>hou)
	{
	    cha=0x9c40-qian+hou;
            cha  = cha*340/2/10000; 
            return  cha;		
	}
	else
	{
          return 0;
	}
	
}
void chashengbo(void)
{
           cashengbo1_2_SET;
	   osDelay(1);//����������
           cashengbo1_2_RESET;
		if(chashengbo_flag1)
		{
		        julia=juli(0,IC_VALUE1);
			chashengbo_flag1=0;
                       // printf("cashengbo_juli:%02f��\r\n",julia);
		}
                
}
extern uint32_t chashengbo_flag1;
extern uint32_t cs_flag1;
extern uint32_t IC_VALUE2;
extern uint32_t IC_VALUE1;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����ش���  ͨ����ʱ��PWM���벶���ȡʱ��Ȼ��������
{
	if(htim->Instance==htim2.Instance)
	{
			switch(htim->Channel)
		{
			
				case HAL_TIM_ACTIVE_CHANNEL_1:
						if(cs_flag1==0)
						{
							       cs_flag1=1;
                                                          __HAL_TIM_DISABLE(htim);                                                                                   
							
							__HAL_TIM_SetCounter(htim,0);
							TIM_RESET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1);  // �����ԭ��������
							TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);// ����Ϊ�½��ز���

							 __HAL_TIM_ENABLE(htim);
						}
						else
						{
                                                        __HAL_TIM_DISABLE(htim);					    //�رն�ʱ��
							IC_VALUE2 = IC_VALUE2+HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // ��ȡ����ֵ
                                                        IC_VALUE1=IC_VALUE2;
                                                        IC_VALUE2=0;
							TIM_RESET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1);  // �����ԭ��������
							TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);// ����Ϊ�����ز���
                                                        __HAL_TIM_ENABLE(htim);
							chashengbo_flag1=1;
                                                        cs_flag1=0;							
						}
				break;
				case HAL_TIM_ACTIVE_CHANNEL_2:
						
				break;
				case HAL_TIM_ACTIVE_CHANNEL_3:
									
				
				
				break;
				case HAL_TIM_ACTIVE_CHANNEL_4:
									
				
				
				break;
				case HAL_TIM_ACTIVE_CHANNEL_CLEARED:
									
				
				
				break;
		}
		
	}
	
}
void chaoshenbo_Task(void const * argument)
{
  /* USER CODE BEGIN chaoshenbo_Task */
  /* Infinite loop */
//  static int flag_juli_buff=0;
//  int flag_juli1=0;
  for(;;)
   {
//       chashengbo();
//       if(julia<1)
//       {
//          flag_juli_buff++;
//       }
//       if(flag_juli_buff>5)
//       {
//         flag_juli1=1;//������ǰ���ϰ���
//         set_database(CHAOSHENG_F,&flag_juli1);
//         flag_juli_buff=0;
//       }
//       if(julia>1)
//       {
//         flag_juli1=0;//������ǰ���ϰ���
//         set_database(CHAOSHENG_F,&flag_juli1);
//         flag_juli_buff=0;
//       }
       osDelay(200);
  }
  /* USER CODE END chaoshenbo_Task */
}
