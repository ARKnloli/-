/**
  ******************************************************************************
  * @file           : chinese_font.h
  * @brief          : 16x16中文字库
  * @date           : 2026-07-03
  ******************************************************************************
  * @attention
  *
  * 包含小车状态显示所需的中文字符
  * 字体大小：16x16像素
  * 取模方式：阴码，逐行式，顺向
  *
  ******************************************************************************
  */

#ifndef __CHINESE_FONT_H
#define __CHINESE_FONT_H

#include "main.h"

/* 字库索引定义 */
typedef enum {
    CH_ZHI = 0,     // 智
    CH_NENG,        // 能
    CH_XIAO,        // 小
    CH_CHE,         // 车
    CH_TING,        // 停
    CH_ZHI2,        // 止
    CH_QIAN,        // 前
    CH_JIN,         // 进
    CH_HOU,         // 后
    CH_TUI,         // 退
    CH_ZUO,         // 左
    CH_ZHUAN,       // 转
    CH_YOU,         // 右
    CH_SU,          // 速
    CH_DU,          // 度
    CH_JIE,         // 接
    CH_SHOU,        // 收
    CH_ZHUANG,      // 状
    CH_TAI,         // 态
    CH_COUNT        // 字符总数
} Chinese_Char_Index;

/* 字库数据声明 */
extern const uint8_t Chinese_Font_16x16[CH_COUNT][32];

/* 显示函数声明 */
void OLED_ShowChinese(uint8_t x, uint8_t y, Chinese_Char_Index index);
void OLED_ShowChineseString(uint8_t x, uint8_t y, const Chinese_Char_Index *str, uint8_t len);

#endif /* __CHINESE_FONT_H */
