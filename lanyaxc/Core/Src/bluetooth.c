/**
  ******************************************************************************
  * @file           : bluetooth.c
  * @brief          : 蓝牙通信模块实现（简化版）
  * @date           : 2026-06-18
  ******************************************************************************
  * @attention
  *
  * JDY-31 蓝牙模块 - 单字节ASCII字符协议
  * 中断中只设置flag，所有业务逻辑在主循环处理
  *
  * 接线说明：
  *   PA9(TX)  → JDY-31 RXD
  *   PA10(RX) ← JDY-31 TXD
  *
  ******************************************************************************
  */

#include "bluetooth.h"
#include "usart.h"

/* 导出变量 */
volatile uint8_t bt_flag = 0;       /* 有新命令标志：1=有命令 */
volatile uint8_t bt_cmd = 0;        /* 收到的命令字符 */
volatile uint8_t bt_rx_count = 0;   /* 接收计数器（调试用） */
volatile uint8_t bt_debug = 0;      /* 调试：显示匹配结果 */

/* 私有变量 */
static uint8_t bt_rx_byte;          /* 单字节接收缓冲 */
static uint8_t bt_speed = 50;       /* 当前速度 (0-100) */
static BT_CallbackFunc bt_callback = NULL;  /* 蓝牙命令回调函数 */

/* 初始化 */
void BT_Init(void)
{
    bt_flag = 0;
    bt_cmd = 0;
    bt_speed = 50;
    HAL_UART_Receive_IT(&huart1, &bt_rx_byte, 1);
}

/* 获取命令（读取后自动清除标志） */
uint8_t BT_GetCmd(void)
{
    uint8_t cmd = bt_cmd;
    bt_flag = 0;
    bt_cmd = 0;
    return cmd;
}

/* 清除命令标志 */
void BT_ClearCmd(void)
{
    bt_flag = 0;
    bt_cmd = 0;
}

/* 获取当前速度 */
uint8_t BT_GetSpeed(void)
{
    return bt_speed;
}

/* 获取最后收到的原始字节 */
uint8_t BT_GetLastByte(void)
{
    return bt_rx_byte;
}

/* 注册蓝牙命令回调函数 */
void BT_RegisterCallback(BT_CallbackFunc callback)
{
    bt_callback = callback;
}

/* 蓝牙处理函数，在主循环中调用 */
void BT_Process(void)
{
    if (bt_flag && bt_callback) {
        bt_callback(bt_cmd, 0);
        bt_flag = 0;
        bt_cmd = 0;
    }
}

/* 串口接收完成回调（中断中调用，只做最少的事） */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        uint8_t rx = bt_rx_byte;

        /* 增加接收计数器（调试用） */
        bt_rx_count++;

        /* 回显测试：收到什么就发回什么 */
        HAL_UART_Transmit(&huart1, &rx, 1, 100);

        /* 调试：显示匹配结果 */
        bt_debug = 0;

        /* 只设置flag，不做任何业务处理 */
        if (rx == CMD_STOP || rx == CMD_FORWARD || rx == CMD_BACKWARD ||
            rx == CMD_TURN_LEFT || rx == CMD_TURN_RIGHT ||
            rx == CMD_SPEED_UP || rx == CMD_SPEED_DOWN ||
            rx == CMD_BEEP) {

            bt_cmd = rx;
            bt_flag = 1;
            bt_debug = 1;

            /* 加减速直接修改速度，但仍需通知主循环 */
            if (rx == CMD_SPEED_UP) {
                bt_speed += 10;
                if (bt_speed > 100) bt_speed = 100;
            } else if (rx == CMD_SPEED_DOWN) {
                if (bt_speed >= 10) bt_speed -= 10;
                else bt_speed = 0;
            }
        }

        /* 重新启动接收 */
        HAL_UART_Receive_IT(&huart1, &bt_rx_byte, 1);
    }
}
