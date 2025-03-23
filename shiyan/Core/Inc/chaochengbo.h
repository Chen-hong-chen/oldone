#ifndef _CHAOCHENGBO_H
#define _CHAOCHENGBO_H
#include "main.h"
#define cashengbo1_2_SET              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
#define cashengbo1_2_RESET            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
#define  chaoshengbo_min    1


float juli(int qian,int hou);
void chashengbo(void);
void chaoshenbo_Task(void const * argument);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
#endif
