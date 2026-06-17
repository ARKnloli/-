/**
  ******************************************************************************
  * @file           : bluetooth.h
  * @brief          : 蓝牙通信模块头文件
  * @date           : 2026-06-16
  ******************************************************************************
  * @attention
  *
  * HC-05/06 蓝牙模块
  * USART1: PA9(TX), PA10(RX), 115200波特率
  *
  * 通信协议:
  *   帧头(2B) + 命令(1B) + 参数(1B) + 校验(1B)
  *   帧头: 0xAA 0x55
  *   校验: 帧头 XOR 命令 XOR 参数
  *
  ******************************************************************************
  */

#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/

/* 帧定义 */
#define BT_FRAME_HEADER1    0xAA
#define BT_FRAME_HEADER2    0x55
#define BT_FRAME_SIZE       5       /* 帧总长度 */

/* 命令定义 */
#define CMD_FORWARD         0x01    /* 前进 */
#define CMD_BACKWARD        0x02    /* 后退 */
#define CMD_TURN_LEFT       0x03    /* 左转 */
#define CMD_TURN_RIGHT      0x04    /* 右转 */
#define CMD_STOP            0x05    /* 停止 */
#define CMD_SPEED           0x06    /* 设置速度 */
#define CMD_GET_STATUS      0x07    /* 获取状态 */
#define CMD_BEEP            0x08    /* 蜂鸣器控制 */

/* 状态定义 */
typedef enum {
    BT_STATE_IDLE = 0,       /* 空闲 */
    BT_STATE_HEADER1,        /* 等待帧头1 */
    BT_STATE_HEADER2,        /* 等待帧头2 */
    BT_STATE_CMD,            /* 等待命令 */
    BT_STATE_PARAM,          /* 等待参数 */
    BT_STATE_CHECK           /* 等待校验 */
} BT_StateTypeDef;

/* 命令回调函数类型 */
typedef void (*BT_CmdCallback)(uint8_t cmd, uint8_t param);

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  蓝牙模块初始化
  * @retval None
  */
void BT_Init(void);

/**
  * @brief  蓝牙接收处理（在主循环中调用）
  * @retval None
  */
void BT_Process(void);

/**
  * @brief  注册命令回调函数
  * @param  callback: 回调函数指针
  * @retval None
  */
void BT_RegisterCallback(BT_CmdCallback callback);

/**
  * @brief  蓝牙发送数据
  * @param  data: 数据指针
  * @param  len: 数据长度
  * @retval None
  */
void BT_SendData(uint8_t *data, uint16_t len);

/**
  * @brief  蓝牙发送状态回复
  * @param  speed: 当前速度
  * @param  dir: 当前方向
  * @retval None
  */
void BT_SendStatus(uint8_t speed, uint8_t dir);

/**
  * @brief  获取当前速度设置
  * @retval 速度值 (0-100)
  */
uint8_t BT_GetSpeed(void);

#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_H */
