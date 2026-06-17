/**
  ******************************************************************************
  * @file           : oled.h
  * @brief          : OLED显示模块头文件 (SSD1306 128x64 I2C)
  * @date           : 2026-06-16
  ******************************************************************************
  * @attention
  *
  * 0.96寸 OLED显示屏
  * I2C1: PB6(SCL), PB7(SDA)
  * 分辨率: 128x64
  * 驱动芯片: SSD1306
  * I2C地址: 0x3C (7位地址)
  *
  ******************************************************************************
  */

#ifndef __OLED_H
#define __OLED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/

/* OLED I2C地址 */
#define OLED_ADDRESS        0x3C

/* OLED屏幕尺寸 */
#define OLED_WIDTH          128
#define OLED_HEIGHT         64

/* 字体大小 */
#define FONT_SIZE_8X16      1
#define FONT_SIZE_6X8       0

/* 字体数据声明 (定义在 oledfont.c 中) */
extern const unsigned char Font8x16[][16];
extern const unsigned char Font6x8[][6];

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  OLED初始化
  * @retval None
  */
void OLED_Init(void);

/**
  * @brief  OLED清屏
  * @retval None
  */
void OLED_Clear(void);

/**
  * @brief  OLED开/关显示
  * @param  on: 1=开, 0=关
  * @retval None
  */
void OLED_DisplayOn(uint8_t on);

/**
  * @brief  设置光标位置
  * @param  x: 列 (0-127)
  * @param  y: 页 (0-7)，每页8像素
  * @retval None
  */
void OLED_SetCursor(uint8_t x, uint8_t y);

/**
  * @brief  显示字符
  * @param  x: 列 (0-127)
  * @param  y: 页 (0-7)
  * @param  ch: 字符
  * @param  size: 字体大小 (FONT_SIZE_8X16 或 FONT_SIZE_6X8)
  * @retval None
  */
void OLED_ShowChar(uint8_t x, uint8_t y, char ch, uint8_t size);

/**
  * @brief  显示字符串
  * @param  x: 列 (0-127)
  * @param  y: 页 (0-7)
  * @param  str: 字符串指针
  * @param  size: 字体大小
  * @retval None
  */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str, uint8_t size);

/**
  * @brief  显示数字
  * @param  x: 列 (0-127)
  * @param  y: 页 (0-7)
  * @param  num: 数字
  * @param  size: 字体大小
  * @retval None
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t size);

/**
  * @brief  显示带符号数字
  * @param  x: 列 (0-127)
  * @param  y: 页 (0-7)
  * @param  num: 数字
  * @param  size: 字体大小
  * @retval None
  */
void OLED_ShowSignedNum(uint8_t x, uint8_t y, int32_t num, uint8_t size);

/**
  * @brief  画像素点
  * @param  x: 列 (0-127)
  * @param  y: 行 (0-63)
  * @param  color: 1=点亮, 0=熄灭
  * @retval None
  */
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color);

/**
  * @brief  清除指定区域
  * @param  x1: 起始列
  * @param  y1: 起始行
  * @param  x2: 结束列
  * @param  y2: 结束行
  * @retval None
  */
void OLED_ClearArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

/**
  * @brief  显示进度条
  * @param  x: 起始列
  * @param  y: 起始页
  * @param  width: 宽度(像素)
  * @param  percent: 进度 (0-100)
  * @retval None
  */
void OLED_ShowProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t percent);

/**
  * @brief  显示图标 (16x16)
  * @param  x: 列
  * @param  y: 页
  * @param  icon: 图标数据指针
  * @retval None
  */
void OLED_ShowIcon(uint8_t x, uint8_t y, const uint8_t *icon);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_H */
