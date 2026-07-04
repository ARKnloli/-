/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
/* 模块1（左侧）通道A */
#define IN1_Pin GPIO_PIN_4
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_5
#define IN2_GPIO_Port GPIOA
/* 模块1（左侧）通道B */
#define IN1B_Pin GPIO_PIN_2
#define IN1B_GPIO_Port GPIOA
#define IN2B_Pin GPIO_PIN_3
#define IN2B_GPIO_Port GPIOA
/* 模块2（右侧）通道A */
#define IN3_Pin GPIO_PIN_6
#define IN3_GPIO_Port GPIOA
#define IN4_Pin GPIO_PIN_7
#define IN4_GPIO_Port GPIOA
/* 模块2（右侧）通道B */
#define IN3B_Pin GPIO_PIN_8
#define IN3B_GPIO_Port GPIOA
#define IN4B_Pin GPIO_PIN_15
#define IN4B_GPIO_Port GPIOA
/* 蜂鸣器 */
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOB
/* ENB使能引脚 */
#define ENB1_Pin GPIO_PIN_1
#define ENB1_GPIO_Port GPIOB
#define ENB2_Pin GPIO_PIN_2
#define ENB2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
