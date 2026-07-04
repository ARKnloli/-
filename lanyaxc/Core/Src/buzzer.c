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

/* 音符频率定义 (Hz) */
#define NOTE_L1   262   /* 低音 do */
#define NOTE_L2   294   /* 低音 re */
#define NOTE_L3   330   /* 低音 mi */
#define NOTE_L4   349   /* 低音 fa */
#define NOTE_L5   392   /* 低音 sol */
#define NOTE_L6   440   /* 低音 la */
#define NOTE_L7   494   /* 低音 si */
#define NOTE_M1   523   /* 中音 do */
#define NOTE_M2   587   /* 中音 re */
#define NOTE_M3   659   /* 中音 mi */
#define NOTE_M4   698   /* 中音 fa */
#define NOTE_M5   784   /* 中音 sol */
#define NOTE_M6   880   /* 中音 la */
#define NOTE_M7   988   /* 中音 si */
#define NOTE_H1   1046  /* 高音 do */
#define NOTE_H2   1175  /* 高音 re */
#define NOTE_H3   1318  /* 高音 mi */
#define NOTE_REST 0     /* 休止符 */

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void Delay_ms(uint16_t ms);
static void Delay_us(uint16_t us);
static void Buzzer_Tone(uint16_t freq, uint16_t duration_ms);

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

/**
  * @brief  前进提示音：上升感（短-短-长）嘟嘟嘟——
  */
void Buzzer_Forward(void)
{
    BUZZER_ON();
    Delay_ms(100);
    BUZZER_OFF();
    Delay_ms(80);
    BUZZER_ON();
    Delay_ms(100);
    BUZZER_OFF();
    Delay_ms(80);
    BUZZER_ON();
    Delay_ms(200);
    BUZZER_OFF();
}

/**
  * @brief  后退提示音：下降感（长-短）嘟——嘟
  */
void Buzzer_Backward(void)
{
    BUZZER_ON();
    Delay_ms(300);
    BUZZER_OFF();
    Delay_ms(100);
    BUZZER_ON();
    Delay_ms(100);
    BUZZER_OFF();
}

/**
  * @brief  左转提示音：单长音（嘟————）
  */
void Buzzer_TurnLeft(void)
{
    BUZZER_ON();
    Delay_ms(400);
    BUZZER_OFF();
}

/**
  * @brief  右转提示音：四声急促（嘟嘟嘟嘟）
  */
void Buzzer_TurnRight(void)
{
    uint8_t i;
    for (i = 0; i < 4; i++) {
        BUZZER_ON();
        Delay_ms(80);
        BUZZER_OFF();
        Delay_ms(80);
    }
}

/**
  * @brief  加速提示音：五声快速（嘟嘟嘟嘟嘟）
  */
void Buzzer_SpeedUp(void)
{
    uint8_t i;
    for (i = 0; i < 5; i++) {
        BUZZER_ON();
        Delay_ms(60);
        BUZZER_OFF();
        Delay_ms(60);
    }
}

/**
  * @brief  减速提示音：超长音（嘟——————）
  */
void Buzzer_SpeedDown(void)
{
    BUZZER_ON();
    Delay_ms(500);
    BUZZER_OFF();
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

/**
  * @brief  微秒级延时函数
  * @param  us: 微秒数
  * @note   基于72MHz主频的粗略延时
  */
static void Delay_us(uint16_t us)
{
    uint16_t i;
    for (i = 0; i < us; i++) {
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
    }
}

/**
  * @brief  用GPIO模拟指定频率的声音
  * @param  freq: 频率 (Hz), 0表示休止符
  * @param  duration_ms: 持续时间 (ms)
  */
static void Buzzer_Tone(uint16_t freq, uint16_t duration_ms)
{
    uint16_t i;
    uint16_t cycle_us;
    uint16_t cycles;

    if (freq == 0) {
        /* 休止符 */
        BUZZER_OFF();
        Delay_ms(duration_ms);
        return;
    }

    /* 计算半周期 (微秒) */
    cycle_us = 500000 / freq;  /* 1000000 / freq / 2 */
    cycles = (duration_ms * 1000) / (cycle_us * 2);

    for (i = 0; i < cycles; i++) {
        BUZZER_ON();
        Delay_us(cycle_us);
        BUZZER_OFF();
        Delay_us(cycle_us);
    }
}

/**
  * @brief  播放音乐：小星星
  * @note   发送 '7' 触发播放
  */
void Buzzer_PlayMusic(void)
{
    /* 小星星乐谱：音符数组 */
    uint16_t melody[] = {
        NOTE_M1, NOTE_M1, NOTE_M5, NOTE_M5, NOTE_M6, NOTE_M6, NOTE_M5, NOTE_REST,
        NOTE_M4, NOTE_M4, NOTE_M3, NOTE_M3, NOTE_M2, NOTE_M2, NOTE_M1, NOTE_REST,
        NOTE_M5, NOTE_M5, NOTE_M4, NOTE_M4, NOTE_M3, NOTE_M3, NOTE_M2, NOTE_REST,
        NOTE_M5, NOTE_M5, NOTE_M4, NOTE_M4, NOTE_M3, NOTE_M3, NOTE_M2, NOTE_REST,
        NOTE_M1, NOTE_M1, NOTE_M5, NOTE_M5, NOTE_M6, NOTE_M6, NOTE_M5, NOTE_REST,
        NOTE_M4, NOTE_M4, NOTE_M3, NOTE_M3, NOTE_M2, NOTE_M2, NOTE_M1
    };

    /* 节拍数组：每个音符的时长（以200ms为单位） */
    uint8_t beats[] = {
        1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2
    };

    uint16_t i;
    uint16_t note_count = sizeof(melody) / sizeof(melody[0]);

    for (i = 0; i < note_count; i++) {
        Buzzer_Tone(melody[i], beats[i] * 200);
        /* 音符间短暂停顿 */
        BUZZER_OFF();
        Delay_ms(50);
    }

    /* 播放结束 */
    BUZZER_OFF();
}
