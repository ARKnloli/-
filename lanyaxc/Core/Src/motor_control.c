/**
  ******************************************************************************
  * @file           : motor_control.c
  * @brief          : 电机控制模块实现
  * @author         : Auto-generated
  * @date           : 2026-06-16
  ******************************************************************************
  * @attention
  *
  * L298N电机驱动控制模块（2个模块，4个电机）
  * 每个模块使用双通道（通道A+通道B），确保4个电机同步转动
  *
  * 接线说明：
  *   模块1（左侧）：
  *     ENA ← PA0 (TIM2_CH1, PWM调速)
  *     ENB ← PB1 (使能通道B)
  *     IN1 ← PA4（通道A方向）
  *     IN2 ← PA5（通道A方向）
  *     IN3 ← PA2（通道B方向）
  *     IN4 ← PA3（通道B方向）
  *     OUT1/OUT2 → 左前电机（通道A）
  *     OUT3/OUT4 → 左后电机（通道B）
  *
  *   模块2（右侧）：
  *     ENA ← PA1 (TIM2_CH2, PWM调速)
  *     ENB ← PB2 (使能通道B)
  *     IN1 ← PA6（通道A方向）
  *     IN2 ← PA7（通道A方向）
  *     IN3 ← PA8（通道B方向）
  *     IN4 ← PA15（通道B方向）
  *     OUT1/OUT2 → 右前电机（通道A）
  *     OUT3/OUT4 → 右后电机（通道B）
  *
  * 控制逻辑：
  *   左电机: IN1=1,IN2=0,IN3=1,IN4=0 → 正转 | IN1=0,IN2=1,IN3=0,IN4=1 → 反转
  *   右电机: IN1=1,IN2=0,IN3=1,IN4=0 → 正转 | IN1=0,IN2=1,IN3=0,IN4=1 → 反转
  *   ENB=HIGH 使能通道B
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
    /* 启动PWM输出（先启动，再配置） */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);  /* ENA (左电机) */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);  /* ENB (右电机) */

    /* 设置初始速度为0 */
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);

    /* ENB使能：PB1=HIGH, PB2=HIGH（使能通道B） */
    HAL_GPIO_WritePin(ENB1_GPIO_Port, ENB1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ENB2_GPIO_Port, ENB2_Pin, GPIO_PIN_SET);

    /* 设置方向引脚为停止状态（通道A+通道B） */
    HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);    // 左通道A
    HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN1B_GPIO_Port, IN1B_Pin, GPIO_PIN_RESET);  // 左通道B
    HAL_GPIO_WritePin(IN2B_GPIO_Port, IN2B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET);    // 右通道A
    HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN3B_GPIO_Port, IN3B_Pin, GPIO_PIN_RESET);  // 右通道B
    HAL_GPIO_WritePin(IN4B_GPIO_Port, IN4B_Pin, GPIO_PIN_RESET);

    /* 记录当前速度 */
    current_left_speed = 0;
    current_right_speed = 0;
}

/**
  * @brief  设置单个电机速度和方向（同时控制通道A和通道B）
  */
void Motor_Set(Motor_IdTypeDef motor, Motor_DirTypeDef dir, uint16_t speed)
{
    /* 限制速度范围 */
    if (speed > MOTOR_PWM_MAX) {
        speed = MOTOR_PWM_MAX;
    }

    /* 设置方向 */
    switch (dir) {
        case MOTOR_DIR_FORWARD:
            if (motor == MOTOR_LEFT) {
                /* 左电机正转：通道A+通道B */
                HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN1B_GPIO_Port, IN1B_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(IN2B_GPIO_Port, IN2B_Pin, GPIO_PIN_RESET);
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
            } else {
                /* 右电机正转：通道A+通道B */
                HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN3B_GPIO_Port, IN3B_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(IN4B_GPIO_Port, IN4B_Pin, GPIO_PIN_RESET);
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
            }
            break;

        case MOTOR_DIR_BACKWARD:
            if (motor == MOTOR_LEFT) {
                /* 左电机反转：通道A+通道B */
                HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(IN1B_GPIO_Port, IN1B_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN2B_GPIO_Port, IN2B_Pin, GPIO_PIN_SET);
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
            } else {
                /* 右电机反转：通道A+通道B */
                HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(IN3B_GPIO_Port, IN3B_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN4B_GPIO_Port, IN4B_Pin, GPIO_PIN_SET);
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
            }
            break;

        case MOTOR_DIR_STOP:
        default:
            if (motor == MOTOR_LEFT) {
                /* 左电机停止：通道A+通道B */
                HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN1B_GPIO_Port, IN1B_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN2B_GPIO_Port, IN2B_Pin, GPIO_PIN_RESET);
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
            } else {
                /* 右电机停止：通道A+通道B */
                HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN3B_GPIO_Port, IN3B_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN4B_GPIO_Port, IN4B_Pin, GPIO_PIN_RESET);
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
            }
            break;
    }
}

/**
  * @brief  停止所有电机（通道A+通道B）
  */
void Motor_StopAll(void)
{
    /* 左电机停止：通道A+通道B */
    HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN1B_GPIO_Port, IN1B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN2B_GPIO_Port, IN2B_Pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);

    /* 右电机停止：通道A+通道B */
    HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN3B_GPIO_Port, IN3B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN4B_GPIO_Port, IN4B_Pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
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
  * @brief  小车前进
  */
void Car_Forward(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_FORWARD, speed);
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_FORWARD, speed);
}

/**
  * @brief  小车后退
  */
void Car_Backward(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_BACKWARD, speed);
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_BACKWARD, speed);
}

/**
  * @brief  小车左转（左侧停，右侧正转）
  */
void Car_TurnLeft(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_STOP, 0);
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_FORWARD, speed);
}

/**
  * @brief  小车右转（左侧正转，右侧停）
  */
void Car_TurnRight(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_FORWARD, speed);
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_STOP, 0);
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

/* 4电机独立控制（2个L298N模块） */
/**
  * @brief  左前电机前进（模块1, OUT1/OUT2）
  */
void Motor_FrontLeftForward(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_FORWARD, speed);
}

/**
  * @brief  左前电机后退（模块1, OUT1/OUT2）
  */
void Motor_FrontLeftBackward(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_BACKWARD, speed);
}

/**
  * @brief  左后电机前进（模块1, OUT3/OUT4）
  */
void Motor_RearLeftForward(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_FORWARD, speed);
}

/**
  * @brief  左后电机后退（模块1, OUT3/OUT4）
  */
void Motor_RearLeftBackward(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_BACKWARD, speed);
}

/**
  * @brief  右前电机前进（模块2, OUT1/OUT2）
  */
void Motor_FrontRightForward(uint16_t speed)
{
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_FORWARD, speed);
}

/**
  * @brief  右前电机后退（模块2, OUT1/OUT2）
  */
void Motor_FrontRightBackward(uint16_t speed)
{
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_BACKWARD, speed);
}

/**
  * @brief  右后电机前进（模块2, OUT3/OUT4）
  */
void Motor_RearRightForward(uint16_t speed)
{
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_FORWARD, speed);
}

/**
  * @brief  右后电机后退（模块2, OUT3/OUT4）
  */
void Motor_RearRightBackward(uint16_t speed)
{
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_BACKWARD, speed);
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
