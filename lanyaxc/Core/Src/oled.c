/**
  ******************************************************************************
  * @file           : oled.c
  * @brief          : OLED显示模块实现 (SSD1306 128x64 I2C)
  * @date           : 2026-06-16
  ******************************************************************************
  * @attention
  *
  * 0.96寸 OLED显示屏 (SSD1306)
  * I2C1: PB6(SCL), PB7(SDA)
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "oled.h"
#include "i2c.h"
#include <string.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

static uint8_t oled_buffer[OLED_WIDTH * OLED_HEIGHT / 8]; /* 显示缓冲区 */

/* SSD1306命令定义 */
#define SSD1306_CMD    0x00    /* 命令 */
#define SSD1306_DATA   0x40    /* 数据 */

/* Private function prototypes -----------------------------------------------*/
static void OLED_WriteCmd(uint8_t cmd);
static void OLED_WriteData(uint8_t data);
static void OLED_SetAddress(uint8_t col, uint8_t page);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  OLED初始化
  */
void OLED_Init(void)
{
    HAL_Delay(100); /* 等待OLED上电稳定 */

    /* SSD1306初始化命令序列 */
    OLED_WriteCmd(0xAE); /* 关闭显示 */
    OLED_WriteCmd(0xD5); /* 设置显示时钟分频 */
    OLED_WriteCmd(0x80); /* 默认值0x80 */
    OLED_WriteCmd(0xA8); /* 设置多路复用率 */
    OLED_WriteCmd(0x3F); /* 1/64 */
    OLED_WriteCmd(0xD3); /* 设置显示偏移 */
    OLED_WriteCmd(0x00); /* 无偏移 */
    OLED_WriteCmd(0x40); /* 设置起始行=0 */
    OLED_WriteCmd(0x8D); /* 设置电荷泵 */
    OLED_WriteCmd(0x14); /* 开启电荷泵 */
    OLED_WriteCmd(0x20); /* 设置内存寻址模式 */
    OLED_WriteCmd(0x02); /* 页寻址模式 */
    OLED_WriteCmd(0xA1); /* 设置段重映射（左右翻转） */
    OLED_WriteCmd(0xC8); /* 设置COM扫描方向（上下翻转） */
    OLED_WriteCmd(0xDA); /* 设置COM引脚硬件配置 */
    OLED_WriteCmd(0x12); /* 默认配置 */
    OLED_WriteCmd(0x81); /* 设置对比度 */
    OLED_WriteCmd(0xCF); /* 对比度值 */
    OLED_WriteCmd(0xD9); /* 设置预充电周期 */
    OLED_WriteCmd(0xF1); /* 默认值 */
    OLED_WriteCmd(0xDB); /* 设置VCOMH取消选择电平 */
    OLED_WriteCmd(0x30); /* 0.83*VCC */
    OLED_WriteCmd(0xA4); /* 全局显示开启（跟随RAM） */
    OLED_WriteCmd(0xA6); /* 正常显示（非反色） */
    OLED_WriteCmd(0xAF); /* 开启显示 */

    OLED_Clear();
}

/**
  * @brief  OLED清屏
  */
void OLED_Clear(void)
{
    uint8_t page, col;

    for (page = 0; page < 8; page++) {
        OLED_WriteCmd(0xB0 + page);  /* 页地址 */
        OLED_WriteCmd(0x00);          /* 列地址低4位 */
        OLED_WriteCmd(0x10);          /* 列地址高4位 */
        for (col = 0; col < 128; col++) {
            OLED_WriteData(0x00);
        }
    }
    OLED_SetCursor(0, 0);
}

/**
  * @brief  OLED开/关显示
  */
void OLED_DisplayOn(uint8_t on)
{
    if (on) {
        OLED_WriteCmd(0xAF); /* 开 */
    } else {
        OLED_WriteCmd(0xAE); /* 关 */
    }
}

/**
  * @brief  设置光标位置
  */
void OLED_SetCursor(uint8_t x, uint8_t y)
{
    OLED_SetAddress(x, y);
}

/**
  * @brief  显示字符 (8x16)
  */
void OLED_ShowChar(uint8_t x, uint8_t y, char ch, uint8_t size)
{
    uint8_t c;
    uint8_t i;

    if (ch < ' ' || ch > '~') {
        ch = ' ';
    }

    c = ch - ' ';

    if (size == FONT_SIZE_8X16) {
        OLED_SetAddress(x, y);
        for (i = 0; i < 8; i++) {
            OLED_WriteData(Font8x16[c][i]);
        }
        OLED_SetAddress(x, y + 1);
        for (i = 0; i < 8; i++) {
            OLED_WriteData(Font8x16[c][i + 8]);
        }
    } else {
        OLED_SetAddress(x, y);
        for (i = 0; i < 6; i++) {
            OLED_WriteData(Font6x8[c][i]);
        }
    }
}

/**
  * @brief  显示字符串
  */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str, uint8_t size)
{
    uint8_t width;

    if (size == FONT_SIZE_8X16) {
        width = 8;
    } else {
        width = 6;
    }

    while (*str != '\0') {
        if (x + width > OLED_WIDTH) {
            x = 0;
            y += (size == FONT_SIZE_8X16) ? 2 : 1;
        }
        if (y + ((size == FONT_SIZE_8X16) ? 2 : 1) > 8) {
            break;
        }
        OLED_ShowChar(x, y, *str, size);
        x += width;
        str++;
    }
}

/**
  * @brief  显示数字
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t size)
{
    char str[12];
    snprintf(str, sizeof(str), "%lu", (unsigned long)num);
    OLED_ShowString(x, y, str, size);
}

/**
  * @brief  显示带符号数字
  */
void OLED_ShowSignedNum(uint8_t x, uint8_t y, int32_t num, uint8_t size)
{
    char str[12];
    snprintf(str, sizeof(str), "%ld", (long)num);
    OLED_ShowString(x, y, str, size);
}

/**
  * @brief  画像素点
  */
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    uint16_t index;
    uint8_t bit_pos;

    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) {
        return;
    }

    index = (y / 8) * OLED_WIDTH + x;
    bit_pos = y % 8;

    if (color) {
        oled_buffer[index] |= (1 << bit_pos);
    } else {
        oled_buffer[index] &= ~(1 << bit_pos);
    }
}

/**
  * @brief  清除指定区域
  */
void OLED_ClearArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    uint8_t x, y;
    for (y = y1; y <= y2; y++) {
        for (x = x1; x <= x2; x++) {
            OLED_DrawPixel(x, y, 0);
        }
    }
}

/**
  * @brief  显示进度条
  * @param  x: 起始列
  * @param  y: 起始页
  * @param  width: 宽度(像素)
  * @param  percent: 进度 (0-100)
  */
void OLED_ShowProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t percent)
{
    uint8_t fill_width;
    uint8_t i;

    if (percent > 100) percent = 100;

    fill_width = (width - 2) * percent / 100;

    /* 第一行（上半部分） */
    OLED_SetAddress(x, y);
    OLED_WriteData(0x7E); /* 左边框 */
    for (i = 0; i < width - 2; i++) {
        if (i < fill_width) {
            OLED_WriteData(0x7E); /* 填充 */
        } else {
            OLED_WriteData(0x42); /* 空 */
        }
    }
    OLED_WriteData(0x7E); /* 右边框 */

    /* 第二行（下半部分） */
    OLED_SetAddress(x, y + 1);
    OLED_WriteData(0x7E); /* 左边框 */
    for (i = 0; i < width - 2; i++) {
        if (i < fill_width) {
            OLED_WriteData(0x7E); /* 填充 */
        } else {
            OLED_WriteData(0x42); /* 空 */
        }
    }
    OLED_WriteData(0x7E); /* 右边框 */
}

/**
  * @brief  显示图标 (16x16, 2页)
  */
void OLED_ShowIcon(uint8_t x, uint8_t y, const uint8_t *icon)
{
    uint8_t i;

    OLED_SetAddress(x, y);
    for (i = 0; i < 16; i++) {
        OLED_WriteData(icon[i]);
    }
    OLED_SetAddress(x, y + 1);
    for (i = 0; i < 16; i++) {
        OLED_WriteData(icon[i + 16]);
    }
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  写命令
  */
static void OLED_WriteCmd(uint8_t cmd)
{
    uint8_t buf[2];
    buf[0] = SSD1306_CMD;
    buf[1] = cmd;
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS << 1, buf, 2, 100);
}

/**
  * @brief  写数据
  */
static void OLED_WriteData(uint8_t data)
{
    uint8_t buf[2];
    buf[0] = SSD1306_DATA;
    buf[1] = data;
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS << 1, buf, 2, 100);
}

/**
  * @brief  设置起始地址（列+页）
  */
static void OLED_SetAddress(uint8_t col, uint8_t page)
{
    OLED_WriteCmd(0xB0 + page);                 /* 页地址 */
    OLED_WriteCmd(0x00 + (col & 0x0F));         /* 列地址低4位 */
    OLED_WriteCmd(0x10 + ((col >> 4) & 0x0F));  /* 列地址高4位 */
}
