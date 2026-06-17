/**
  ******************************************************************************
  * @file           : buzzer.h
  * @brief          : 蜂鸣器控制模块头文件
  * @author         : Auto-generated
  * @date           : 2026-06-16
  ******************************************************************************
  * @attention
  *
  * 低电平触发蜂鸣器
  * PB0 → BUZZER
  * 高电平 = 静音，低电平 = 响
  *
  ******************************************************************************
  */

#ifndef __BUZZER_H
#define __BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* 蜂鸣器引脚定义 */
#define BUZZER_PORT         BUZZER_GPIO_Port
#define BUZZER_PIN          BUZZER_Pin

/* 蜂鸣器状态宏 */
#define BUZZER_ON()         HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET)   /* 响 */
#define BUZZER_OFF()        HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET)     /* 静音 */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  蜂鸣器初始化
  * @note   初始状态为静音（高电平）
  * @retval None
  */
void Buzzer_Init(void);

/**
  * @brief  蜂鸣器开启（响）
  * @retval None
  */
void Buzzer_On(void);

/**
  * @brief  蜂鸣器关闭（静音）
  * @retval None
  */
void Buzzer_Off(void);

/**
  * @brief  蜂鸣器翻转状态
  * @retval None
  */
void Buzzer_Toggle(void);

/**
  * @brief  蜂鸣器短响（用于提示音）
  * @param  ms: 响的时间（毫秒）
  * @retval None
  */
void Buzzer_Beep(uint16_t ms);

/**
  * @brief  蜂鸣器响两声（成功提示）
  * @retval None
  */
void Buzzer_Success(void);

/**
  * @brief  蜂鸣器响三声（错误提示）
  * @retval None
  */
void Buzzer_Error(void);

#ifdef __cplusplus
}
#endif

#endif /* __BUZZER_H */
