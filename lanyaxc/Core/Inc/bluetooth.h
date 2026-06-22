/**
  ******************************************************************************
  * @file           : bluetooth.h
  * @brief          : 蓝牙通信模块头文件（简化版）
  * @date           : 2026-06-18
  ******************************************************************************
  * @attention
  *
  * JDY-31 蓝牙模块
  * USART1: PA9(TX) → JDY-31 RXD, PA10(RX) ← JDY-31 TXD, 9600波特率
  *
  * 协议：单字节ASCII字符
  *   '0' = 停止    '1' = 前进    '2' = 后退
  *   '3' = 左转    '4' = 右转    '5' = 加速    '6' = 减速
  *
  ******************************************************************************
  */

#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* 命令定义（ASCII字符） */
#define CMD_STOP        '0'
#define CMD_FORWARD     '1'
#define CMD_BACKWARD    '2'
#define CMD_TURN_LEFT   '3'
#define CMD_TURN_RIGHT  '4'
#define CMD_SPEED_UP    '5'
#define CMD_SPEED_DOWN  '6'
#define CMD_BEEP        '7'

/* 导出变量 */
extern volatile uint8_t bt_flag;       /* 蓝牙命令标志 */
extern volatile uint8_t bt_cmd;        /* 蓝牙收到的命令 */
extern volatile uint8_t bt_rx_count;   /* 接收计数器（调试用） */
extern volatile uint8_t bt_debug;      /* 调试：显示匹配结果 */

/* 回调函数类型定义 */
typedef void (*BT_CallbackFunc)(uint8_t cmd, uint8_t param);

/* 函数声明 */
void BT_Init(void);
uint8_t BT_GetCmd(void);
void BT_ClearCmd(void);
uint8_t BT_GetSpeed(void);
uint8_t BT_GetLastByte(void);  /* 获取最后收到的原始字节 */
void BT_RegisterCallback(BT_CallbackFunc callback);  /* 注册蓝牙命令回调函数 */
void BT_Process(void);  /* 蓝牙处理函数，在主循环中调用 */

#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_H */
