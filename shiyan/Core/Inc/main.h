/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RUN_LED_Pin GPIO_PIN_13
#define RUN_LED_GPIO_Port GPIOC
#define ROAD_LED_Pin GPIO_PIN_0
#define ROAD_LED_GPIO_Port GPIOC
#define ROAD_LED_EXIT_Pin GPIO_PIN_1
#define ROAD_LED_EXIT_GPIO_Port GPIOC
#define ROAD_LED_EXIT_EXTI_IRQn EXTI1_IRQn
#define Echo_Pin GPIO_PIN_0
#define Echo_GPIO_Port GPIOA
#define Trigc_Pin GPIO_PIN_1
#define Trigc_GPIO_Port GPIOA
#define GPS_TX_Pin GPIO_PIN_2
#define GPS_TX_GPIO_Port GPIOA
#define GPS_RX_Pin GPIO_PIN_3
#define GPS_RX_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_4
#define CS_GPIO_Port GPIOA
#define W25_SCK_Pin GPIO_PIN_5
#define W25_SCK_GPIO_Port GPIOA
#define W25_MISO_Pin GPIO_PIN_6
#define W25_MISO_GPIO_Port GPIOA
#define W25_MOSI_Pin GPIO_PIN_7
#define W25_MOSI_GPIO_Port GPIOA
#define LED_ERROR_Pin GPIO_PIN_5
#define LED_ERROR_GPIO_Port GPIOC
#define MCU_RC522_NIRST_Pin GPIO_PIN_1
#define MCU_RC522_NIRST_GPIO_Port GPIOB
#define MCU_RC522_SCK_Pin GPIO_PIN_2
#define MCU_RC522_SCK_GPIO_Port GPIOB
#define MCU_RC522_MISO_Pin GPIO_PIN_7
#define MCU_RC522_MISO_GPIO_Port GPIOE
#define MCU_RC522_MOSI_Pin GPIO_PIN_8
#define MCU_RC522_MOSI_GPIO_Port GPIOE
#define MCU_RC522_NSS_Pin GPIO_PIN_9
#define MCU_RC522_NSS_GPIO_Port GPIOE
#define MCU_RC522_IRQ_Pin GPIO_PIN_10
#define MCU_RC522_IRQ_GPIO_Port GPIOE
#define TX_4G_Pin GPIO_PIN_10
#define TX_4G_GPIO_Port GPIOC
#define RX_4G_Pin GPIO_PIN_11
#define RX_4G_GPIO_Port GPIOC
#define MPU6050SCL_Pin GPIO_PIN_6
#define MPU6050SCL_GPIO_Port GPIOB
#define MPU6050SDA_Pin GPIO_PIN_7
#define MPU6050SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define EC80_UART    &huart4
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
