/**
  ******************************************************************************
  * @file           : buzzer.c
  * @brief          : 蜂鸣器控制模块实现
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

/* Includes ------------------------------------------------------------------*/
#include "buzzer.h"

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void Delay_ms(uint16_t ms);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  蜂鸣器初始化
  */
void Buzzer_Init(void)
{
    /* 初始状态为静音（高电平） */
    BUZZER_OFF();
}

/**
  * @brief  蜂鸣器开启（响）
  */
void Buzzer_On(void)
{
    BUZZER_ON();
}

/**
  * @brief  蜂鸣器关闭（静音）
  */
void Buzzer_Off(void)
{
    BUZZER_OFF();
}

/**
  * @brief  蜂鸣器翻转状态
  */
void Buzzer_Toggle(void)
{
    HAL_GPIO_TogglePin(BUZZER_PORT, BUZZER_PIN);
}

/**
  * @brief  蜂鸣器短响（用于提示音）
  */
void Buzzer_Beep(uint16_t ms)
{
    BUZZER_ON();
    Delay_ms(ms);
    BUZZER_OFF();
}

/**
  * @brief  蜂鸣器响两声（成功提示）
  */
void Buzzer_Success(void)
{
    BUZZER_ON();
    Delay_ms(100);
    BUZZER_OFF();
    Delay_ms(100);
    BUZZER_ON();
    Delay_ms(100);
    BUZZER_OFF();
}

/**
  * @brief  蜂鸣器响三声（错误提示）
  */
void Buzzer_Error(void)
{
    uint8_t i;
    for (i = 0; i < 3; i++) {
        BUZZER_ON();
        Delay_ms(150);
        BUZZER_OFF();
        Delay_ms(150);
    }
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  简单延时函数（粗略延时）
  * @param  ms: 毫秒数
  * @note   基于72MHz主频的粗略延时，不精确
  */
static void Delay_ms(uint16_t ms)
{
    uint16_t i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 7200; j++) {
            __NOP();
        }
    }
}
