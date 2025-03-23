/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "chaochengbo.h"
#include "usart.h"
#include "mpu6050.h"
#include "stdlib.h"
#include "W25Qx.h"
#include <string.h>
#include "tim.h"
#include "pubdata.h"
#include "rc522.h"
#include "gps.h"
#include "ec800_at.h"
#include <stdio.h>
#include <cJSON.h>
#include "iwdg.h"
#include "ec800_at.h"
#include "ec800_mqtt.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

extern old_guai_zhang oldone;//拐杖的数据库
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
EC80ReceiveType  receivebuff_4G;//创建数据接收4G
int light_buff=0;//可以让灯及时的显示在手机上  为什么（中断中不可用互斥锁）  在循环中及时的上传
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
    
osMessageQId EC800_4G_ReceQueueHandle;//创建4G的消息队列


osThreadId chaoshenboTaskHandle;
osThreadId Mpu6050TaskHandle;
osThreadId cardTaskHandle;
osThreadId Task_check1Handle;
osThreadId Gps_taskHandle;
osThreadId EC800_receive_taskHandle;
osThreadId EC800_send_taskHandle;
osThreadId IDWG_taskHandle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osTimerId sendTimerHandle;
osMutexId Mutex_flagHandle;
osMutexId Mutex_ziyuanHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Task_check(void const * argument);
void IDWG_task(void const * argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void sendCallback(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of Mutex_flag */
  osMutexDef(Mutex_flag);
  Mutex_flagHandle = osMutexCreate(osMutex(Mutex_flag));

  /* definition and creation of Mutex_ziyuan */
  osMutexDef(Mutex_ziyuan);
  Mutex_ziyuanHandle = osMutexCreate(osMutex(Mutex_ziyuan));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of sendTimer */
  osTimerDef(sendTimer, sendCallback);
  sendTimerHandle = osTimerCreate(osTimer(sendTimer), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  osMessageQDef(EC80_ReceQueue, 3, EC80ReceiveType);
  EC800_4G_ReceQueueHandle = osMessageCreate(osMessageQ(EC80_ReceQueue), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  
  memset(&oldone,ZE,sizeof(old_guai_zhang));
  /* Infinite loop */
  taskENTER_CRITICAL();
  /* definition and creation of chaoshenboTask */
  osThreadDef(chaoshenboTask, chaoshenbo_Task, osPriorityNormal, 0, 200);
  chaoshenboTaskHandle = osThreadCreate(osThread(chaoshenboTask), NULL);

  /* definition and creation of Mpu6050Task */
  osThreadDef(Mpu6050Task, Mpu6050_Task, osPriorityAboveNormal, 0, 300);
  Mpu6050TaskHandle = osThreadCreate(osThread(Mpu6050Task), NULL);

  /* definition and creation of cardTask */
  osThreadDef(cardTask, Card_Task, osPriorityNormal, 0, 300);
  cardTaskHandle = osThreadCreate(osThread(cardTask), NULL);

  /* definition and creation of Task_check1 */
  osThreadDef(Task_check1, Task_check, osPriorityNormal, 0, 400);
  Task_check1Handle = osThreadCreate(osThread(Task_check1), NULL);
   
  osThreadDef(Gps_task1, Gps_task, osPriorityNormal, 0, 300);
  Gps_taskHandle = osThreadCreate(osThread(Gps_task1), NULL);
  
  osThreadDef(EC800_receive_task1, EC800_receive_task, osPriorityNormal, 0, 800);
  EC800_receive_taskHandle = osThreadCreate(osThread(EC800_receive_task1), NULL);
  
  
  osThreadDef(EC800_send_task1, EC800_send_task, osPriorityNormal, 0, 800);
  EC800_send_taskHandle = osThreadCreate(osThread(EC800_send_task1), NULL);
  
  
  
  osThreadDef(IDWG_task1,IDWG_task, osPriorityNormal, 0, 200);
  IDWG_taskHandle = osThreadCreate(osThread(IDWG_task1), NULL);
  
  vTaskDelete(defaultTaskHandle);
  
   taskEXIT_CRITICAL();





  /* USER CODE END StartDefaultTask */
}

/* sendCallback function */
void sendCallback(void const * argument)
{
  /* USER CODE BEGIN sendCallback */
     mqtt_json_send();
     at_send_OK(MQTT,NULL,NULL);//3秒
  /* USER CODE END sendCallback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//外部中断控制ROAD
{
 
  if(light_buff==ON)
  {
     light_buff=ZE;
     HAL_GPIO_WritePin(ROAD_LED_GPIO_Port, ROAD_LED_Pin, GPIO_PIN_RESET);
  }
  else
  {
    light_buff=ON;
    HAL_GPIO_WritePin(ROAD_LED_GPIO_Port, ROAD_LED_Pin, GPIO_PIN_SET);//亮
  }
  
  
}
/* USER CODE BEGIN Header_Task_check */
/**
* @brief Function implementing the Task_check1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_check */
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    printf("the stack of %s is overflow\r\n",pcTaskName);
}
void Task_check(void const * argument)
{
  /* USER CODE BEGIN Task_check */
  

  
  char taskzha[400]={0};
  memset(taskzha,0,400);
  /* Infinite loop */
  for(;;)
  { 
     HAL_GPIO_WritePin(RUN_LED_GPIO_Port, RUN_LED_Pin, GPIO_PIN_RESET);
    osDelay(500);
     HAL_GPIO_WritePin(RUN_LED_GPIO_Port, RUN_LED_Pin, GPIO_PIN_SET);
     osDelay(500);
     set_database(LIGHT_F,&light_buff);//控制手电筒 数据库
    vTaskList(taskzha);
    // printf("task_name   task_state  priority   stack  tasK_num\n");
    // printf("%s\n", taskzha); 
     
  }
  /* USER CODE END Task_check */
}

void IDWG_task(void const * argument)
{
  /* USER CODE BEGIN Task_check */
  /* Infinite loop */
  for(;;)
  {
   
     osDelay(3000);
   HAL_IWDG_Refresh(&hiwdg);  

  }
  /* USER CODE END Task_check */
}
/* USER CODE END Application */

