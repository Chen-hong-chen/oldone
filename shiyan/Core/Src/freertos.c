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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

extern old_guai_zhang oldone;//拐杖的数据库
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
EC80ReceiveType  receivebuff_4G;//创建数据接收4G
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
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osMutexId Mutex_flagHandle;
osMutexId Mutex_ziyuanHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Task_check(void const * argument);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

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
  osThreadDef(Mpu6050Task, Mpu6050_Task, osPriorityIdle, 0, 300);
  Mpu6050TaskHandle = osThreadCreate(osThread(Mpu6050Task), NULL);

  /* definition and creation of cardTask */
  osThreadDef(cardTask, Card_Task, osPriorityIdle, 0, 300);
  cardTaskHandle = osThreadCreate(osThread(cardTask), NULL);

  /* definition and creation of Task_check1 */
  osThreadDef(Task_check1, Task_check, osPriorityIdle, 0, 400);
  Task_check1Handle = osThreadCreate(osThread(Task_check1), NULL);
   
  osThreadDef(Gps_task1, Gps_task, osPriorityIdle, 0, 300);
  Gps_taskHandle = osThreadCreate(osThread(Gps_task1), NULL);
  
  osThreadDef(EC800_receive_task1, EC800_receive_task, osPriorityIdle, 0, 400);
  EC800_receive_taskHandle = osThreadCreate(osThread(EC800_receive_task1), NULL);
  
  
  osThreadDef(EC800_send_task1, EC800_send_task, osPriorityIdle, 0, 400);
  EC800_send_taskHandle = osThreadCreate(osThread(EC800_send_task1), NULL);
  
  taskEXIT_CRITICAL();
  vTaskDelete(NULL);




  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE BEGIN Header_Task_check */
/**
* @brief Function implementing the Task_check1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_check */
void Task_check(void const * argument)
{
  /* USER CODE BEGIN Task_check */
  char taskzha[400]={0};
  memset(taskzha,0,400);
  /* Infinite loop */
  for(;;)
  {
    vTaskList(taskzha);
     printf("task_name   task_state  priority   stack  tasK_num\n");
     printf("%s\n", taskzha); 
     HAL_GPIO_WritePin(RUN_LED_GPIO_Port, RUN_LED_Pin, GPIO_PIN_RESET);
     osDelay(500);
     HAL_GPIO_WritePin(RUN_LED_GPIO_Port, RUN_LED_Pin, GPIO_PIN_SET);
     osDelay(500);

  }
  /* USER CODE END Task_check */
}
/* USER CODE END Application */

