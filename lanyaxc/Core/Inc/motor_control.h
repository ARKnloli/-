/**
  ******************************************************************************
  * @file           : motor_control.h
  * @brief          : 电机控制模块头文件
  * @author         : Auto-generated
  * @date           : 2026-06-16
  ******************************************************************************
  * @attention
  *
  * L298N电机驱动控制模块
  * 左电机: IN1(PA4), IN2(PA5), ENA(PA0-TIM2_CH1)
  * 右电机: IN3(PA6), IN4(PA7), ENB(PA1-TIM2_CH2)
  *
  ******************************************************************************
  */

#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/

/* 电机方向枚举 */
typedef enum {
    MOTOR_DIR_STOP = 0,     /* 停止 */
    MOTOR_DIR_FORWARD,      /* 正转 */
    MOTOR_DIR_BACKWARD      /* 反转 */
} Motor_DirTypeDef;

/* 电机编号枚举 */
typedef enum {
    MOTOR_LEFT = 0,         /* 左电机 */
    MOTOR_RIGHT             /* 右电机 */
} Motor_IdTypeDef;

/* Exported constants --------------------------------------------------------*/

/* PWM最大值（与TIM2 Period一致） */
#define MOTOR_PWM_MAX        999

/* 默认速度（50%） */
#define MOTOR_DEFAULT_SPEED  500

/* 软启动配置 */
#define MOTOR_SOFT_START_STEP    50     /* 每步增加的PWM值 */
#define MOTOR_SOFT_START_DELAY   10     /* 每步延时(ms) */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  电机控制模块初始化
  * @note   启动PWM输出，初始速度为0
  * @retval None
  */
void Motor_Init(void);

/**
  * @brief  设置单个电机速度和方向
  * @param  motor: 电机编号 (MOTOR_LEFT 或 MOTOR_RIGHT)
  * @param  dir: 方向 (MOTOR_DIR_STOP, MOTOR_DIR_FORWARD, MOTOR_DIR_BACKWARD)
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  * @retval None
  */
void Motor_Set(Motor_IdTypeDef motor, Motor_DirTypeDef dir, uint16_t speed);

/**
  * @brief  停止所有电机
  * @retval None
  */
void Motor_StopAll(void);

/**
  * @brief  左电机前进
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  * @retval None
  */
void Motor_LeftForward(uint16_t speed);

/**
  * @brief  左电机后退
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  * @retval None
  */
void Motor_LeftBackward(uint16_t speed);

/**
  * @brief  右电机前进
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  * @retval None
  */
void Motor_RightForward(uint16_t speed);

/**
  * @brief  右电机后退
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  * @retval None
  */
void Motor_RightBackward(uint16_t speed);

/**
  * @brief  小车前进
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  * @retval None
  */
void Car_Forward(uint16_t speed);

/**
  * @brief  小车后退
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  * @retval None
  */
void Car_Backward(uint16_t speed);

/**
  * @brief  小车左转
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  * @retval None
  */
void Car_TurnLeft(uint16_t speed);

/**
  * @brief  小车右转
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  * @retval None
  */
void Car_TurnRight(uint16_t speed);

/**
  * @brief  小车停止
  * @retval None
  */
void Car_Stop(void);

/* 4电机独立控制（2个L298N模块） */
/**
  * @brief  左前电机前进
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  */
void Motor_FrontLeftForward(uint16_t speed);

/**
  * @brief  左前电机后退
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  */
void Motor_FrontLeftBackward(uint16_t speed);

/**
  * @brief  左后电机前进
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  */
void Motor_RearLeftForward(uint16_t speed);

/**
  * @brief  左后电机后退
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  */
void Motor_RearLeftBackward(uint16_t speed);

/**
  * @brief  右前电机前进
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  */
void Motor_FrontRightForward(uint16_t speed);

/**
  * @brief  右前电机后退
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  */
void Motor_FrontRightBackward(uint16_t speed);

/**
  * @brief  右后电机前进
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  */
void Motor_RearRightForward(uint16_t speed);

/**
  * @brief  右后电机后退
  * @param  speed: 速度 (0 ~ MOTOR_PWM_MAX)
  */
void Motor_RearRightBackward(uint16_t speed);

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_CONTROL_H */
