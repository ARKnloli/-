/**
  ******************************************************************************
  * @file           : bluetooth.c
  * @brief          : 蓝牙通信模块实现
  * @date           : 2026-06-16
  ******************************************************************************
  * @attention
  *
  * HC-05/06 蓝牙模块 - 串口中断接收 + 状态机解析
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bluetooth.h"
#include "usart.h"

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

static BT_StateTypeDef bt_state = BT_STATE_IDLE;   /* 接收状态机 */
static uint8_t bt_rx_cmd = 0;                       /* 接收到的命令 */
static uint8_t bt_rx_param = 0;                     /* 接收到的参数 */
static uint8_t bt_rx_check = 0;                     /* 接收到的校验 */
static uint8_t bt_speed = 50;                       /* 当前速度 (0-100) */

static BT_CmdCallback bt_callback = NULL;           /* 命令回调函数 */

/* 接收缓冲区 */
static uint8_t bt_rx_byte;                          /* 单字节接收缓冲 */

/* Private function prototypes -----------------------------------------------*/
static uint8_t BT_CalculateCheck(uint8_t cmd, uint8_t param);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  蓝牙模块初始化
  */
void BT_Init(void)
{
    /* 启动串口中断接收 */
    HAL_UART_Receive_IT(&huart1, &bt_rx_byte, 1);

    bt_state = BT_STATE_IDLE;
    bt_speed = 50;
}

/**
  * @brief  蓝牙接收处理（在主循环中调用）
  */
void BT_Process(void)
{
    /* 状态机处理在中断回调中完成 */
    /* 此处可用于超时处理等扩展功能 */
}

/**
  * @brief  注册命令回调函数
  */
void BT_RegisterCallback(BT_CmdCallback callback)
{
    bt_callback = callback;
}

/**
  * @brief  蓝牙发送数据
  */
void BT_SendData(uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit(&huart1, data, len, 100);
}

/**
  * @brief  蓝牙发送状态回复
  * 格式: 0xAA 0x55 0x80 速度 方向 校验
  */
void BT_SendStatus(uint8_t speed, uint8_t dir)
{
    uint8_t tx_buf[6];
    tx_buf[0] = BT_FRAME_HEADER1;
    tx_buf[1] = BT_FRAME_HEADER2;
    tx_buf[2] = 0x80;           /* 状态回复命令 */
    tx_buf[3] = speed;
    tx_buf[4] = dir;
    tx_buf[5] = tx_buf[0] ^ tx_buf[1] ^ tx_buf[2] ^ tx_buf[3] ^ tx_buf[4];

    BT_SendData(tx_buf, 6);
}

/**
  * @brief  获取当前速度
  */
uint8_t BT_GetSpeed(void)
{
    return bt_speed;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  计算校验值
  */
static uint8_t BT_CalculateCheck(uint8_t cmd, uint8_t param)
{
    return BT_FRAME_HEADER1 ^ BT_FRAME_HEADER2 ^ cmd ^ param;
}

/* Weak 函数：串口接收完成回调（重写HAL的回调） */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        uint8_t rx = bt_rx_byte;

        switch (bt_state) {
            case BT_STATE_IDLE:
                if (rx == BT_FRAME_HEADER1) {
                    bt_state = BT_STATE_HEADER1;
                }
                break;

            case BT_STATE_HEADER1:
                if (rx == BT_FRAME_HEADER2) {
                    bt_state = BT_STATE_HEADER2;
                } else {
                    bt_state = BT_STATE_IDLE;
                }
                break;

            case BT_STATE_HEADER2:
                bt_rx_cmd = rx;
                bt_state = BT_STATE_CMD;
                break;

            case BT_STATE_CMD:
                bt_rx_param = rx;
                bt_state = BT_STATE_PARAM;
                break;

            case BT_STATE_PARAM:
                bt_rx_check = rx;
                bt_state = BT_STATE_CHECK;

                /* 校验验证 */
                if (bt_rx_check == BT_CalculateCheck(bt_rx_cmd, bt_rx_param)) {
                    /* 校验通过，处理命令 */
                    switch (bt_rx_cmd) {
                        case CMD_SPEED:
                            bt_speed = bt_rx_param;
                            if (bt_speed > 100) bt_speed = 100;
                            break;

                        case CMD_BEEP:
                            /* 蜂鸣器控制由main.c中的回调处理 */
                            break;

                        default:
                            break;
                    }

                    /* 调用回调函数 */
                    if (bt_callback != NULL) {
                        bt_callback(bt_rx_cmd, bt_rx_param);
                    }
                }

                bt_state = BT_STATE_IDLE;
                break;

            default:
                bt_state = BT_STATE_IDLE;
                break;
        }

        /* 继续接收下一字节 */
        HAL_UART_Receive_IT(&huart1, &bt_rx_byte, 1);
    }
}
