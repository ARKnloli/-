/**
  ******************************************************************************
  * @file           : motor_control.c
  * @brief          : 电机控制模块实现
  * @author         : Auto-generated
  * @date           : 2026-06-16
  ******************************************************************************
  * @attention
  *
  * L298N电机驱动控制模块
  * 控制逻辑:
  *   左电机: IN1=1,IN2=0 → 正转 | IN1=0,IN2=1 → 反转 | IN1=0,IN2=0 → 停止
  *   右电机: IN3=1,IN4=0 → 正转 | IN3=0,IN4=1 → 反转 | IN3=0,IN4=0 → 停止
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "motor_control.h"
#include "tim.h"

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint16_t current_left_speed = 0;   /* 当前左电机速度 */
static uint16_t current_right_speed = 0;  /* 当前右电机速度 */

/* Private function prototypes -----------------------------------------------*/
static void Motor_SoftStart(Motor_IdTypeDef motor, uint16_t target_speed);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  电机控制模块初始化
  */
void Motor_Init(void)
{
    /* 停止所有电机 */
    Motor_StopAll();

    /* 启动PWM输出 */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);  /* 启动ENA (左电机) */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);  /* 启动ENB (右电机) */
}

/**
  * @brief  设置单个电机速度和方向
  */
void Motor_Set(Motor_IdTypeDef motor, Motor_DirTypeDef dir, uint16_t speed)
{
    GPIO_TypeDef *gpio_port;
    uint16_t pin_fwd, pin_bwd;

    /* 限制速度范围 */
    if (speed > MOTOR_PWM_MAX) {
        speed = MOTOR_PWM_MAX;
    }

    /* 选择电机对应的引脚 */
    if (motor == MOTOR_LEFT) {
        gpio_port = IN1_GPIO_Port;
        pin_fwd = IN1_Pin;      /* PA4 - IN1 */
        pin_bwd = IN2_Pin;      /* PA5 - IN2 */

        /* 设置PWM占空比 */
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
    } else {
        gpio_port = IN3_GPIO_Port;
        pin_fwd = IN3_Pin;      /* PA6 - IN3 */
        pin_bwd = IN4_Pin;      /* PA7 - IN4 */

        /* 设置PWM占空比 */
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
    }

    /* 设置方向 */
    switch (dir) {
        case MOTOR_DIR_FORWARD:
            HAL_GPIO_WritePin(gpio_port, pin_fwd, GPIO_PIN_SET);
            HAL_GPIO_WritePin(gpio_port, pin_bwd, GPIO_PIN_RESET);
            break;

        case MOTOR_DIR_BACKWARD:
            HAL_GPIO_WritePin(gpio_port, pin_fwd, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(gpio_port, pin_bwd, GPIO_PIN_SET);
            break;

        case MOTOR_DIR_STOP:
        default:
            HAL_GPIO_WritePin(gpio_port, pin_fwd, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(gpio_port, pin_bwd, GPIO_PIN_RESET);
            /* 停止时PWM设为0 */
            if (motor == MOTOR_LEFT) {
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
            } else {
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
            }
            break;
    }
}

/**
  * @brief  停止所有电机
  */
void Motor_StopAll(void)
{
    /* 左电机停止 */
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_STOP, 0);

    /* 右电机停止 */
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_STOP, 0);
}

/**
  * @brief  左电机前进
  */
void Motor_LeftForward(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_FORWARD, speed);
}

/**
  * @brief  左电机后退
  */
void Motor_LeftBackward(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_BACKWARD, speed);
}

/**
  * @brief  右电机前进
  */
void Motor_RightForward(uint16_t speed)
{
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_FORWARD, speed);
}

/**
  * @brief  右电机后退
  */
void Motor_RightBackward(uint16_t speed)
{
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_BACKWARD, speed);
}

/**
  * @brief  小车前进（带软启动）
  */
void Car_Forward(uint16_t speed)
{
    Motor_SoftStart(MOTOR_LEFT, speed);
    Motor_SoftStart(MOTOR_RIGHT, speed);
}

/**
  * @brief  小车后退（带软启动）
  */
void Car_Backward(uint16_t speed)
{
    Motor_SoftStart(MOTOR_LEFT, speed);
    Motor_SoftStart(MOTOR_RIGHT, speed);
}

/**
  * @brief  小车左转（原地左转，带软启动）
  */
void Car_TurnLeft(uint16_t speed)
{
    Motor_SoftStart(MOTOR_LEFT, speed);
    Motor_SoftStart(MOTOR_RIGHT, speed);
}

/**
  * @brief  小车右转（原地右转，带软启动）
  */
void Car_TurnRight(uint16_t speed)
{
    Motor_SoftStart(MOTOR_LEFT, speed);
    Motor_SoftStart(MOTOR_RIGHT, speed);
}

/**
  * @brief  小车停止
  */
void Car_Stop(void)
{
    Motor_StopAll();
    current_left_speed = 0;
    current_right_speed = 0;
}

/**
  * @brief  电机软启动（渐增占空比，避免电流冲击）
  */
static void Motor_SoftStart(Motor_IdTypeDef motor, uint16_t target_speed)
{
    uint16_t *current_speed;
    uint16_t step;
    uint16_t new_speed;

    /* 获取当前速度 */
    if (motor == MOTOR_LEFT) {
        current_speed = &current_left_speed;
    } else {
        current_speed = &current_right_speed;
    }

    /* 计算步进值 */
    if (target_speed > *current_speed) {
        /* 加速 */
        while (*current_speed < target_speed) {
            step = target_speed - *current_speed;
            if (step > MOTOR_SOFT_START_STEP) {
                step = MOTOR_SOFT_START_STEP;
            }
            new_speed = *current_speed + step;

            /* 根据当前方向设置速度 */
            if (motor == MOTOR_LEFT) {
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, new_speed);
            } else {
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, new_speed);
            }

            *current_speed = new_speed;
            HAL_Delay(MOTOR_SOFT_START_DELAY);
        }
    } else {
        /* 减速（立即生效） */
        *current_speed = target_speed;
        if (motor == MOTOR_LEFT) {
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, target_speed);
        } else {
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, target_speed);
        }
    }
}
