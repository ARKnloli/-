/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 蓝牙小车主程序
  * @author         : 蓝牙小车项目组
  * @date           : 2026-07-03
  ******************************************************************************
  * @brief           程序功能概述
  *
  * 本程序实现一个基于STM32F103C8T6的蓝牙遥控小车，主要功能包括：
  *   1. 蓝牙通信：通过JDY-31蓝牙模块接收手机APP发送的控制指令
  *   2. 电机控制：通过2个L298N模块控制4个直流电机（前进/后退/左转/右转/停止）
  *   3. OLED显示：在0.96寸OLED屏幕上显示中文状态信息（状态、速度、接收计数）
  *   4. 蜂鸣器提示：开机提示音、停止提示音
  *   5. PWM调速：通过TIM2输出PWM信号控制电机速度
  *
  * 程序流程：
  *   系统初始化 → 外设初始化 → 模块初始化 → 显示初始化界面 → 主循环
  *   主循环：蓝牙命令处理 → OLED显示更新
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"       /* 主头文件：包含HAL库、GPIO定义等 */
#include "i2c.h"        /* I2C通信：用于OLED显示 */
#include "tim.h"        /* 定时器：用于PWM输出控制电机速度 */
#include "usart.h"      /* 串口通信：用于蓝牙模块通信 */
#include "gpio.h"       /* GPIO配置：电机方向、蜂鸣器、ENB使能等 */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor_control.h"   /* 电机控制：控制4个直流电机的转向和速度 */
#include "buzzer.h"          /* 蜂鸣器：开机提示音、停止提示音 */
#include "bluetooth.h"       /* 蓝牙通信：接收手机APP的控制命令 */
#include "oled.h"            /* OLED显示：显示状态、速度等信息 */
#include "chinese_font.h"    /* 中文字库：用于OLED显示中文 */
#include <stdio.h>           /* 标准输入输出：sprintf函数 */
#include <string.h>          /* 字符串处理 */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/**
 * 当前小车状态变量
 * 这些变量在蓝牙回调函数中被修改，在主循环中用于更新OLED显示
 */
static uint16_t current_speed = 999;  /* 当前速度 (0-999)，初始全速 */
static uint8_t current_dir = 0;       /* 当前方向：0=停止,1=前进,2=后退,3=左转,4=右转 */
static uint8_t display_update = 1;    /* 显示更新标志：1=需要更新OLED */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void BT_CommandCallback(uint8_t cmd, uint8_t param);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* 初始化各个功能模块 */
  Motor_Init();     /* 初始化电机：启动PWM，设置ENB使能，方向引脚置低 */
  Buzzer_Init();    /* 初始化蜂鸣器：PB0置高电平（静音） */
  BT_Init();        /* 初始化蓝牙：启动UART中断接收 */
  OLED_Init();      /* 初始化OLED：发送SSD1306初始化命令序列 */

  OLED_Clear();     /* 清屏 */

  /**
   * 显示OLED初始化界面
   * 屏幕布局：
   *   第0页(0-15像素): 智能小车（标题）
   *   第2页(16-31像素): 状态：停止
   *   第4页(32-47像素): 速度：100%
   *   第6页(48-63像素): 接收：0
   */

  /* 显示中文标题：智能小车 */
  OLED_ShowChinese(0, 0, CH_ZHI);      // 智
  OLED_ShowChinese(16, 0, CH_NENG);     // 能
  OLED_ShowChinese(32, 0, CH_XIAO);     // 小
  OLED_ShowChinese(48, 0, CH_CHE);      // 车

  /* 显示状态：停止 */
  OLED_ShowChinese(0, 2, CH_ZHUANG);    // 状
  OLED_ShowChinese(16, 2, CH_TAI);      // 态
  OLED_ShowString(32, 2, ": ", FONT_SIZE_8X16);
  OLED_ShowChinese(48, 2, CH_TING);     // 停
  OLED_ShowChinese(64, 2, CH_ZHI2);     // 止

  /* 显示速度 */
  OLED_ShowChinese(0, 4, CH_SU);        // 速
  OLED_ShowChinese(16, 4, CH_DU);       // 度
  OLED_ShowString(32, 4, ": 100%  ", FONT_SIZE_6X8);

  /* 显示接收计数 */
  OLED_ShowChinese(0, 6, CH_JIE);       // 接
  OLED_ShowChinese(16, 6, CH_SHOU);     // 收
  OLED_ShowString(32, 6, ": 0     ", FONT_SIZE_6X8);

  Buzzer_Success();  /* 开机提示音：响两声表示初始化成功 */
  BT_RegisterCallback(BT_CommandCallback);  /* 注册蓝牙命令回调函数 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /**
     * 主循环任务1：处理蓝牙命令
     * 检查是否有新的蓝牙命令，如果有则调用回调函数执行对应动作
     */
    BT_Process();

    /**
     * 主循环任务2：更新OLED显示
     * 只在 display_update 标志为1时更新，避免频繁刷新导致闪烁
     * display_update 在蓝牙回调函数中被置1
     */
    if (display_update) {
        display_update = 0;  /* 清除更新标志 */

        /* === 显示状态行（第2页，y=2）=== */
        OLED_ShowChinese(0, 2, CH_ZHUANG);    // 状
        OLED_ShowChinese(16, 2, CH_TAI);      // 态
        OLED_ShowString(32, 2, ": ", FONT_SIZE_8X16);

        /* 根据 current_dir 显示对应的方向中文 */
        switch (current_dir) {
            case 0:  // 停止
                OLED_ShowChinese(48, 2, CH_TING);
                OLED_ShowChinese(64, 2, CH_ZHI2);
                OLED_ShowString(80, 2, "    ", FONT_SIZE_8X16);  /* 清除多余字符 */
                break;
            case 1:  // 前进
                OLED_ShowChinese(48, 2, CH_QIAN);
                OLED_ShowChinese(64, 2, CH_JIN);
                OLED_ShowString(80, 2, "    ", FONT_SIZE_8X16);
                break;
            case 2:  // 后退
                OLED_ShowChinese(48, 2, CH_HOU);
                OLED_ShowChinese(64, 2, CH_TUI);
                OLED_ShowString(80, 2, "    ", FONT_SIZE_8X16);
                break;
            case 3:  // 左转
                OLED_ShowChinese(48, 2, CH_ZUO);
                OLED_ShowChinese(64, 2, CH_ZHUAN);
                OLED_ShowString(80, 2, "    ", FONT_SIZE_8X16);
                break;
            case 4:  // 右转
                OLED_ShowChinese(48, 2, CH_YOU);
                OLED_ShowChinese(64, 2, CH_ZHUAN);
                OLED_ShowString(80, 2, "    ", FONT_SIZE_8X16);
                break;
        }

        /* === 显示速度行（第4页，y=4）=== */
        OLED_ShowChinese(0, 4, CH_SU);        // 速
        OLED_ShowChinese(16, 4, CH_DU);       // 度
        OLED_ShowString(32, 4, ": ", FONT_SIZE_8X16);

        /* 将速度值(0-999)转换为百分比(0-100%)显示 */
        uint8_t speed_percent = (current_speed * 100) / MOTOR_PWM_MAX;
        char speed_str[8];
        sprintf(speed_str, "%3d%%", speed_percent);  /* 格式化为 "XX%" */
        OLED_ShowString(48, 4, speed_str, FONT_SIZE_8X16);

        /* === 显示接收计数行（第6页，y=6）=== */
        OLED_ShowChinese(0, 6, CH_JIE);       // 接
        OLED_ShowChinese(16, 6, CH_SHOU);     // 收
        OLED_ShowString(32, 6, ": ", FONT_SIZE_8X16);

        /* 显示蓝牙接收的总命令数（调试用） */
        char rx_str[8];
        sprintf(rx_str, "%d   ", bt_rx_count);
        OLED_ShowString(48, 6, rx_str, FONT_SIZE_8X16);
    }

    /* USER CODE END 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief  蓝牙命令回调函数
  * @note   当蓝牙收到有效命令时，由BT_Process()调用此函数
  * @param  cmd: 命令字符（'0'-'7'）
  * @param  param: 参数（当前未使用，保留）
  *
  * 蓝牙协议：
  *   '0' (0x30) - 停止    '1' (0x31) - 前进
  *   '2' (0x32) - 后退    '3' (0x33) - 左转
  *   '4' (0x34) - 右转    '5' (0x35) - 加速
  *   '6' (0x36) - 减速    '7' (0x37) - 蜂鸣器
  */
void BT_CommandCallback(uint8_t cmd, uint8_t param)
{
    switch (cmd) {
        case CMD_FORWARD:           /* '1' - 前进 */
            Car_Forward(current_speed);  /* 两侧电机都正转 */
            current_dir = 1;
            display_update = 1;
            Buzzer_Forward();           /* 前进提示音：两声短促 */
            break;
        case CMD_BACKWARD:          /* '2' - 后退 */
            Car_Backward(current_speed); /* 两侧电机都反转 */
            current_dir = 2;
            display_update = 1;
            Buzzer_Backward();          /* 后退提示音：一声长响 */
            break;
        case CMD_TURN_LEFT:         /* '3' - 左转 */
            Car_TurnLeft(current_speed); /* 左侧停，右侧正转 */
            current_dir = 3;
            display_update = 1;
            Buzzer_TurnLeft();          /* 左转提示音：一声短促 */
            break;
        case CMD_TURN_RIGHT:        /* '4' - 右转 */
            Car_TurnRight(current_speed); /* 左侧正转，右侧停 */
            current_dir = 4;
            display_update = 1;
            Buzzer_TurnRight();         /* 右转提示音：两声快速 */
            break;
        case CMD_STOP:              /* '0' - 停止 */
            Car_Stop();                 /* 所有电机停止 */
            current_dir = 0;
            display_update = 1;
            Buzzer_Beep(100);           /* 蜂鸣器短响100ms提示停止 */
            break;
        case CMD_SPEED_UP:          /* '5' - 加速 */
            current_speed += 100;       /* 速度增加100（约10%） */
            if (current_speed > MOTOR_PWM_MAX) current_speed = MOTOR_PWM_MAX;
            display_update = 1;
            Buzzer_SpeedUp();           /* 加速提示音：三声渐快 */
            break;
        case CMD_SPEED_DOWN:        /* '6' - 减速 */
            if (current_speed >= 100) current_speed -= 100;
            else current_speed = 0;     /* 速度减少100，最小为0 */
            display_update = 1;
            Buzzer_SpeedDown();         /* 减速提示音：一声低沉 */
            break;
        case CMD_BEEP:              /* '7' - 播放音乐 */
            Buzzer_PlayMusic();         /* 播放小星星音乐 */
            break;
        default:
            break;
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error source line number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
