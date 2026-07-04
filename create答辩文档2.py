#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
蓝牙小车项目答辩问题与解答文档（详细版）
"""

from docx import Document
from docx.shared import Pt, Cm, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.style import WD_STYLE_TYPE

def create_document():
    doc = Document()

    # 设置文档样式
    style = doc.styles['Normal']
    style.font.name = '微软雅黑'
    style.font.size = Pt(11)

    # 标题
    title = doc.add_heading('STM32蓝牙智能小车', 0)
    title.alignment = WD_ALIGN_PARAGRAPH.CENTER

    subtitle = doc.add_paragraph('答辩问题与详细解答')
    subtitle.alignment = WD_ALIGN_PARAGRAPH.CENTER
    subtitle.runs[0].font.size = Pt(16)

    doc.add_paragraph('')

    # 一、项目概述
    doc.add_heading('一、项目概述与评分对应', level=1)

    doc.add_paragraph('本项目是基于STM32F103C8T6的蓝牙智能小车，通过手机APP蓝牙控制小车运动，实现前进、后退、左转、右转、停止等基本功能，并支持速度调节和蜂鸣器提示音。')

    doc.add_paragraph('')

    # 评分标准对应表
    score_table = doc.add_table(rows=5, cols=3)
    score_table.style = 'Table Grid'

    score_data = [
        ('评分项', '分值', '实现功能'),
        ('基本功能：指令执行', '30分', '前进、后退、左转、右转、停止'),
        ('提示音功能', '20分', '不同操作播放不同提示音'),
        ('扩展功能：速度调整', '15分', '通过蓝牙实现加速/减速'),
        ('机动分数', '5分', 'OLED显示、调试功能等'),
    ]

    for i, (item, score, func) in enumerate(score_data):
        score_table.rows[i].cells[0].text = item
        score_table.rows[i].cells[1].text = score
        score_table.rows[i].cells[2].text = func

    doc.add_paragraph('')

    # 二、硬件系统设计
    doc.add_heading('二、硬件系统设计', level=1)

    # 2.1 系统架构
    doc.add_heading('2.1 系统整体架构', level=2)
    doc.add_paragraph('''
系统采用分层架构设计：

┌─────────────────────────────────────────────────────┐
│                    手机APP                           │
│              (蓝牙串口调试助手)                       │
└───────────────────────┬─────────────────────────────┘
                        │ 蓝牙无线通信
                        ▼
┌─────────────────────────────────────────────────────┐
│              JDY-31蓝牙模块                          │
│           (USART通信，9600波特率)                     │
└───────────────────────┬─────────────────────────────┘
                        │ UART串口
                        ▼
┌─────────────────────────────────────────────────────┐
│            STM32F103C8T6 主控芯片                    │
│    ┌──────────┬──────────┬──────────┬──────────┐    │
│    │ USART1   │ TIM2     │ I2C1     │ GPIO     │    │
│    │ 蓝牙通信 │ PWM输出  │ OLED显示 │ 蜂鸣器   │    │
│    └────┬─────┴────┬─────┴────┬─────┴────┬─────┘    │
└─────────┼──────────┼──────────┼──────────┼──────────┘
          │          │          │          │
          ▼          ▼          ▼          ▼
    ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐
    │ JDY-31  │ │ L298N   │ │ OLED    │ │ 蜂鸣器  │
    │ 蓝牙模块│ │ 电机驱动│ │ 0.96寸  │ │ 有源    │
    └─────────┘ └────┬────┘ └─────────┘ └─────────┘
                     │
          ┌──────────┴──────────┐
          ▼                     ▼
    ┌──────────┐          ┌──────────┐
    │ 左侧电机 │          │ 右侧电机 │
    │ (2个并联)│          │ (2个并联)│
    └──────────┘          └──────────┘
''')

    # 2.2 电源系统
    doc.add_heading('2.2 电源系统设计', level=2)
    doc.add_paragraph('''
【电源方案】
• 电池：6节1.5V干电池串联 = 9V
• L298N供电：9V电池+ → L298N的12V输入端
• STM32供电：L298N的5V输出 → STM32的5V引脚
• 蓝牙/OLED供电：STM32的3.3V引脚

【共地设计】
电池GND → L298N GND → STM32 GND → 蓝牙GND → OLED GND
（所有模块必须共地，否则通信不稳定）

【滤波电容】
每个L298N模块并联100~470μF电解电容：
• 正极接电池+
• 负极接GND
• 作用：储能、滤波、稳定电压
• 注意：电解电容有极性，接反会爆炸！
''')

    # 2.3 蓝牙模块连接
    doc.add_heading('2.3 蓝牙模块连接（USART通信）', level=2)
    doc.add_paragraph('''
【重要说明】蓝牙模块使用USART通信，不是I2C！

【硬件连接】
┌─────────────┬─────────────┬─────────────────────┐
│ JDY-31引脚  │ STM32引脚   │ 说明                │
├─────────────┼─────────────┼─────────────────────┤
│ VCC         │ 3.3V        │ ⚠️ 不能接5V！       │
│ GND         │ GND         │ 共地                │
│ TXD         │ PA10 (RX)   │ 蓝牙发送→STM32接收  │
│ RXD         │ PA9  (TX)   │ STM32发送→蓝牙接收  │
└─────────────┴─────────────┴─────────────────────┘

【为什么要交叉连接？】
• TX是发送端，RX是接收端
• A设备的TX要连到B设备的RX
• A设备的RX要连到B设备的TX
• 这样才能实现双向通信

【通信参数】
• 波特率：9600
• 数据位：8位
• 停止位：1位
• 校验位：无
• 模式：透传模式
''')

    # 2.4 OLED连接
    doc.add_heading('2.4 OLED显示模块连接（I2C通信）', level=2)
    doc.add_paragraph('''
【重要说明】OLED使用I2C通信，不是USART！

【硬件连接】
┌─────────────┬─────────────┬─────────────────────┐
│ OLED引脚    │ STM32引脚   │ 说明                │
├─────────────┼─────────────┼─────────────────────┤
│ VCC         │ 3.3V        │ 供电                │
│ GND         │ GND         │ 共地                │
│ SCL         │ PB6 (I2C1)  │ I2C时钟线           │
│ SDA         │ PB7 (I2C1)  │ I2C数据线           │
└─────────────┴─────────────┴─────────────────────┘

【I2C通信特点】
• 只需2根线（SCL+SDA）
• 支持多设备挂载
• 速度：400kHz（快速模式）
• 设备地址：0x3C（OLED的固定地址）

【I2C与USART的区别】
┌──────────────┬─────────────────┬─────────────────┐
│ 特性         │ I2C             │ USART           │
├──────────────┼─────────────────┼─────────────────┤
│ 线数         │ 2根（SCL+SDA）  │ 2根（TX+RX）    │
│ 通信方式     │ 半双工          │ 全双工          │
│ 设备数量     │ 多个设备        │ 点对点          │
│ 速度         │ 100-400kHz      │ 9600-115200bps  │
│ 本项目用途   │ OLED显示        │ 蓝牙通信        │
└──────────────┴─────────────────┴─────────────────┘
''')

    # 2.5 电机驱动
    doc.add_heading('2.5 L298N电机驱动模块连接', level=2)
    doc.add_paragraph('''
【L298N模块简介】
• 双H桥驱动，可同时控制2个电机
• 支持PWM调速
• 输入电压：5-35V
• 输出电流：2A（每通道）

【模块1（左侧电机）连接】
┌─────────────┬─────────────┬─────────────────────┐
│ L298N引脚   │ STM32引脚   │ 说明                │
├─────────────┼─────────────┼─────────────────────┤
│ 12V         │ 电池+ (9V)  │ 电机电源            │
│ GND         │ 电池-       │ 共地                │
│ 5V          │ STM32 5V    │ 跳线帽在，输出5V    │
│ ENA         │ PA0         │ TIM2_CH1，PWM调速   │
│ IN1         │ PA4         │ 方向控制1           │
│ IN2         │ PA5         │ 方向控制2           │
│ OUT1        │ 左前电机+   │ 正极                │
│ OUT2        │ 左前电机-   │ 负极                │
│ OUT3        │ 左后电机+   │ 正极                │
│ OUT4        │ 左后电机-   │ 负极                │
└─────────────┴─────────────┴─────────────────────┘

【模块2（右侧电机）连接】
┌─────────────┬─────────────┬─────────────────────┐
│ L298N引脚   │ STM32引脚   │ 说明                │
├─────────────┼─────────────┼─────────────────────┤
│ 12V         │ 电池+ (9V)  │ 电机电源            │
│ GND         │ 电池-       │ 共地                │
│ 5V          │ 不接        │ 跳线帽可拔          │
│ ENA         │ PA1         │ TIM2_CH2，PWM调速   │
│ IN1         │ PA6         │ 方向控制1           │
│ IN2         │ PA7         │ 方向控制2           │
│ OUT1        │ 右前电机-   │ ⚠️ 负极（交换接线） │
│ OUT2        │ 右前电机+   │ ⚠️ 正极（交换接线） │
│ OUT3        │ 右后电机+   │ 正极                │
│ OUT4        │ 右后电机-   │ 负极                │
└─────────────┴─────────────┴─────────────────────┘

【为什么右前电机要交换接线？】
• 左右电机安装方向是镜像的
• 不交换的话，右前电机方向会相反
• 交换后，发送"前进"指令时4个轮子都向前转

【L298N控制逻辑】
┌──────┬──────┬────────────┐
│ IN1  │ IN2  │ 电机状态   │
├──────┼──────┼────────────┤
│  0   │  0   │ 停止       │
│  1   │  0   │ 正转       │
│  0   │  1   │ 反转       │
│  1   │  1   │ 制动       │
└──────┴──────┴────────────┘
''')

    # 2.6 蜂鸣器连接
    doc.add_heading('2.6 蜂鸣器连接', level=2)
    doc.add_paragraph('''
【硬件连接】
┌─────────────┬─────────────┬─────────────────────┐
│ 蜂鸣器引脚  │ STM32引脚   │ 说明                │
├─────────────┼─────────────┼─────────────────────┤
│ VCC         │ 3.3V或5V    │ 供电                │
│ GND         │ GND         │ 共地                │
│ I/O         │ PB0         │ 低电平触发          │
└─────────────┴─────────────┴─────────────────────┘

【控制方式】
• 低电平触发（PB0=0时响）
• 高电平静音（PB0=1时停）
• 有源蜂鸣器，无需PWM
''')

    # 2.7 完整引脚分配
    doc.add_heading('2.7 完整引脚分配表', level=2)

    pin_table = doc.add_table(rows=13, cols=4)
    pin_table.style = 'Table Grid'

    pin_data = [
        ('STM32引脚', '功能', '连接模块', '说明'),
        ('PA0', 'TIM2_CH1', 'L298N左侧ENA', '左侧电机PWM调速'),
        ('PA1', 'TIM2_CH2', 'L298N右侧ENA', '右侧电机PWM调速'),
        ('PA4', 'GPIO', 'L298N左侧IN1', '左侧电机方向1'),
        ('PA5', 'GPIO', 'L298N左侧IN2', '左侧电机方向2'),
        ('PA6', 'GPIO', 'L298N右侧IN1', '右侧电机方向1'),
        ('PA7', 'GPIO', 'L298N右侧IN2', '右侧电机方向2'),
        ('PA9', 'USART1_TX', '蓝牙RXD', 'STM32发送→蓝牙接收'),
        ('PA10', 'USART1_RX', '蓝牙TXD', '蓝牙发送→STM32接收'),
        ('PB0', 'GPIO', '蜂鸣器I/O', '低电平触发'),
        ('PB6', 'I2C1_SCL', 'OLED SCL', 'I2C时钟线'),
        ('PB7', 'I2C1_SDA', 'OLED SDA', 'I2C数据线'),
        ('PA13/14', 'SWD', 'ST-Link', '调试接口'),
    ]

    for i, (pin, func, module, desc) in enumerate(pin_data):
        pin_table.rows[i].cells[0].text = pin
        pin_table.rows[i].cells[1].text = func
        pin_table.rows[i].cells[2].text = module
        pin_table.rows[i].cells[3].text = desc

    doc.add_paragraph('')

    # 三、软件系统设计
    doc.add_heading('三、软件系统设计', level=1)

    # 3.1 蓝牙通信协议
    doc.add_heading('3.1 蓝牙通信协议设计（对应30分基本功能）', level=2)
    doc.add_paragraph('''
【协议设计】
采用极简的单字节ASCII字符协议，每个字符对应一个功能：

┌──────────┬──────────┬──────────────┬─────────────────────┐
│ 字符     │ HEX值    │ 功能         │ 小车动作            │
├──────────┼──────────┼──────────────┼─────────────────────┤
│ '0'      │ 0x30     │ 停止         │ 4个电机全部停止     │
│ '1'      │ 0x31     │ 前进         │ 4个电机同时正转     │
│ '2'      │ 0x32     │ 后退         │ 4个电机同时反转     │
│ '3'      │ 0x33     │ 左转         │ 左侧停，右侧正转   │
│ '4'      │ 0x34     │ 右转         │ 左侧正转，右侧停   │
│ '5'      │ 0x35     │ 加速         │ 速度+10%           │
│ '6'      │ 0x36     │ 减速         │ 速度-10%           │
│ '7'      │ 0x37     │ 蜂鸣器       │ 响200ms            │
└──────────┴──────────┴──────────────┴─────────────────────┘

【为什么选择单字节协议？】
1. 简单可靠：无帧头帧尾，无校验，减少出错可能
2. 响应快：单字节传输，延迟低
3. 易调试：ASCII字符可打印，便于测试
4. 兼容性好：所有蓝牙串口APP都支持

【对比5字节帧协议的改进】
参考项目使用5字节帧协议（帧头+数据+校验+帧尾），本项目简化为单字节：
• 减少了协议解析复杂度
• 提高了通信可靠性
• 便于调试和测试
''')

    # 3.2 蓝牙数据接收
    doc.add_heading('3.2 蓝牙数据接收实现', level=2)
    doc.add_paragraph('''
【接收方式】中断接收

【实现代码】
// 串口接收完成回调（中断中调用）
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        uint8_t rx = bt_rx_byte;

        // 增加接收计数器（调试用）
        bt_rx_count++;

        // 只设置flag，不做任何业务处理
        if (rx == CMD_STOP || rx == CMD_FORWARD || ...) {
            bt_cmd = rx;      // 保存命令
            bt_flag = 1;      // 设置标志
        }

        // 重新启动接收
        HAL_UART_Receive_IT(&huart1, &bt_rx_byte, 1);
    }
}

【设计要点】
1. 中断中只做最少的事：读取数据、设置标志
2. 复杂处理放在主循环中
3. 避免中断阻塞，保证实时性
4. 每次接收完成后重新启动接收

【调试方法】
• OLED显示RX计数和原始字节
• 蜂鸣器响表示收到数据
• 串口回显测试
''')

    # 3.3 电机控制
    doc.add_heading('3.3 电机控制实现（对应30分基本功能）', level=2)
    doc.add_paragraph('''
【控制逻辑】

1. 前进（发送'1'）：
   • 左电机：IN1=1, IN2=0 → 正转
   • 右电机：IN1=1, IN2=0 → 正转（因接线交换，实际向前）
   • 结果：4个轮子都向前转

2. 后退（发送'2'）：
   • 左电机：IN1=0, IN2=1 → 反转
   • 右电机：IN1=0, IN2=1 → 反转（因接线交换，实际向后）
   • 结果：4个轮子都向后转

3. 左转（发送'3'）：
   • 左电机：停止（IN1=0, IN2=0）
   • 右电机：正转（IN1=1, IN2=0）
   • 结果：小车向左转弯

4. 右转（发送'4'）：
   • 左电机：正转（IN1=1, IN2=0）
   • 右电机：停止（IN1=0, IN2=0）
   • 结果：小车向右转弯

5. 停止（发送'0'）：
   • 左电机：停止（IN1=0, IN2=0）
   • 右电机：停止（IN1=0, IN2=0）
   • 结果：小车停止

【PWM调速】
• 使用TIM2的PWM功能
• 频率：1kHz
• 分辨率：0-999
• 默认速度：500（50%）

【代码实现】
void Car_Forward(uint16_t speed)
{
    Motor_Set(MOTOR_LEFT, MOTOR_DIR_FORWARD, speed);
    Motor_Set(MOTOR_RIGHT, MOTOR_DIR_FORWARD, speed);
}

void Motor_Set(Motor_IdTypeDef motor, Motor_DirTypeDef dir, uint16_t speed)
{
    // 设置PWM占空比
    if (motor == MOTOR_LEFT) {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
    } else {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
    }

    // 设置方向引脚
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
            HAL_GPIO_WritePin(gpio_port, pin_fwd, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(gpio_port, pin_bwd, GPIO_PIN_RESET);
            break;
    }
}
''')

    # 3.4 提示音功能
    doc.add_heading('3.4 提示音功能实现（对应20分）', level=2)
    doc.add_paragraph('''
【功能说明】
不同操作播放不同提示音，提供声音反馈。

【提示音设计】
┌──────────────┬──────────────┬─────────────────────┐
│ 操作         │ 提示音       │ 实现方式            │
├──────────────┼──────────────┼─────────────────────┤
│ 收到任意指令 │ 短响一声     │ 响100ms             │
│ 停止指令     │ 长响一声     │ 响200ms             │
│ 蜂鸣器指令   │ 连续响       │ 响500ms             │
│ 系统启动     │ 响一声       │ 自检提示            │
└──────────────┴──────────────┴─────────────────────┘

【控制代码】
void Buzzer_Beep(uint16_t duration_ms)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);  // 低电平，响
    HAL_Delay(duration_ms);
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);    // 高电平，停
}

【蓝牙控制蜂鸣器】
case CMD_BEEP:  // 收到'7'
    Buzzer_Beep(500);  // 响500ms
    break;
''')

    # 3.5 速度调整
    doc.add_heading('3.5 速度调整功能实现（对应15分扩展功能）', level=2)
    doc.add_paragraph('''
【功能说明】
通过蓝牙指令实现小车速度的动态调整。

【速度控制】
• 发送'5'：加速，速度+10%
• 发送'6'：减速，速度-10%
• 速度范围：0% - 100%

【实现代码】
// 在蓝牙中断中处理
if (rx == CMD_SPEED_UP) {
    bt_speed += 10;
    if (bt_speed > 100) bt_speed = 100;
} else if (rx == CMD_SPEED_DOWN) {
    if (bt_speed >= 10) bt_speed -= 10;
    else bt_speed = 0;
}

// 在主循环中应用速度
case CMD_SPEED_UP:
    current_speed += 100;  // PWM值+100
    if (current_speed > MOTOR_PWM_MAX) current_speed = MOTOR_PWM_MAX;
    break;
case CMD_SPEED_DOWN:
    if (current_speed >= 100) current_speed -= 100;
    else current_speed = 0;
    break;

【OLED显示速度】
• 实时显示当前速度百分比
• 格式：SPD: 50%
''')

    # 四、答辩问题与解答
    doc.add_heading('四、答辩常见问题与详细解答', level=1)

    # 问题1：系统架构
    doc.add_heading('问题1：请介绍一下你的蓝牙小车系统的整体架构？', level=2)
    doc.add_paragraph('''
【回答】
本系统采用"手机APP + 蓝牙模块 + STM32主控 + 电机驱动"的架构：

1. 手机APP端：
   • 使用蓝牙串口调试助手
   • 发送单字节ASCII字符指令
   • 界面简单，操作方便

2. 蓝牙通信层：
   • JDY-31蓝牙模块
   • USART通信，9600波特率
   • 透传模式，数据直接转发

3. 主控层：
   • STM32F103C8T6微控制器
   • 72MHz主频，64KB Flash
   • 负责指令解析和外设控制

4. 驱动层：
   • L298N电机驱动模块 x2
   • 控制4个直流电机
   • 支持PWM调速

5. 输出层：
   • 电机：执行运动
   • OLED：显示状态
   • 蜂鸣器：声音提示

【数据流】
手机APP → 蓝牙 → USART中断 → 主循环解析 → 电机/OLED/蜂鸣器
''')

    # 问题2：蓝牙通信
    doc.add_heading('问题2：蓝牙通信是如何实现的？', level=2)
    doc.add_paragraph('''
【回答】

1. 硬件连接：
   • JDY-31蓝牙模块
   • VCC接3.3V（不能接5V）
   • TXD接STM32的PA10（RX）
   • RXD接STM32的PA9（TX）
   • 注意：TX/RX要交叉连接

2. 通信参数：
   • 波特率：9600
   • 数据位：8位
   • 停止位：1位
   • 校验位：无

3. 数据接收：
   • 使用USART中断接收
   • 每次接收1个字节
   • 中断中只设置标志，不处理业务
   • 复杂处理放在主循环

4. 协议设计：
   • 单字节ASCII字符
   • 每个字符对应一个功能
   • 无帧格式，简单可靠

5. 调试方法：
   • OLED显示RX计数
   • 串口回显测试
   • 蜂鸣器确认收到

【为什么选择USART而不是I2C？】
• 蓝牙模块只支持USART
• USART是全双工，可同时收发
• 蓝牙通信是点对点，不需要I2C的多设备支持
''')

    # 问题3：I2C与USART
    doc.add_heading('问题3：蓝牙模块和OLED分别用什么通信方式？有什么区别？', level=2)
    doc.add_paragraph('''
【回答】

1. 蓝牙模块：USART通信
   • 引脚：PA9(TX)、PA10(RX)
   • 特点：全双工、点对点
   • 用途：与手机APP通信

2. OLED显示：I2C通信
   • 引脚：PB6(SCL)、PB7(SDA)
   • 特点：半双工、多设备支持
   • 用途：显示状态信息

3. 两者区别：
┌──────────────┬─────────────────┬─────────────────┐
│ 特性         │ USART           │ I2C             │
├──────────────┼─────────────────┼─────────────────┤
│ 线数         │ 2根（TX+RX）    │ 2根（SCL+SDA）  │
│ 通信方式     │ 全双工          │ 半双工          │
│ 设备数量     │ 点对点          │ 多个设备        │
│ 速度         │ 9600bps         │ 400kHz          │
│ 本项目用途   │ 蓝牙通信        │ OLED显示        │
│ 复杂度       │ 简单            │ 需要设备地址    │
└──────────────┴─────────────────┴─────────────────┘

4. 为什么这样选择？
   • 蓝牙模块只支持USART，没得选
   • OLED支持I2C和SPI，选择I2C更简单
   • 两种通信方式互不干扰
''')

    # 问题4：电机控制
    doc.add_heading('问题4：电机控制是如何实现的？', level=2)
    doc.add_paragraph('''
【回答】

1. 硬件方案：
   • 2个L298N模块，控制4个电机
   • 模块1控制左侧2个电机（并联）
   • 模块2控制右侧2个电机（并联）

2. 控制原理：
   • L298N是双H桥驱动
   • 通过IN1/IN2控制电机方向
   • 通过ENA的PWM控制电机速度

3. 方向控制：
┌──────┬──────┬────────────┐
│ IN1  │ IN2  │ 电机状态   │
├──────┼──────┼────────────┤
│  0   │  0   │ 停止       │
│  1   │  0   │ 正转       │
│  0   │  1   │ 反转       │
│  1   │  1   │ 制动       │
└──────┴──────┴────────────┘

4. 速度控制：
   • 使用TIM2的PWM功能
   • 频率：1kHz
   • 分辨率：0-999
   • 通过改变占空比调速

5. 前进实现：
   • 左电机：IN1=1, IN2=0（正转）
   • 右电机：IN1=1, IN2=0（正转）
   • 因为右电机接线交换，实际向前

6. 左转实现：
   • 左电机：停止（IN1=0, IN2=0）
   • 右电机：正转（IN1=1, IN2=0）
   • 结果：小车向左转弯
''')

    # 问题5：右前电机接线
    doc.add_heading('问题5：为什么右前电机要交换接线？', level=2)
    doc.add_paragraph('''
【回答】

1. 问题原因：
   • 左右电机的物理安装方向是镜像的
   • 左电机正转时，轮子向前转
   • 右电机正转时，轮子向后转（因为安装方向相反）
   • 这导致前进时右侧轮子反转

2. 解决方案：
   • 交换右前电机的接线
   • OUT1接负极（原来接正极）
   • OUT2接正极（原来接负极）
   • 这样当L298N输出正转信号时，右前电机实际向前转

3. 效果：
   • 发送'1'（前进）时，4个轮子都向前转
   • 发送'2'（后退）时，4个轮子都向后转
   • 左转/右转时，差速转向正确

4. 为什么不修改代码？
   • 硬件交换接线更简单
   • 代码逻辑保持统一
   • 便于维护和理解
''')

    # 问题6：遇到的问题
    doc.add_heading('问题6：项目开发过程中遇到了哪些问题？如何解决的？', level=2)
    doc.add_paragraph('''
【回答】

1. 蓝牙通信不工作（2026-06-18）
   现象：APP发送数据，STM32无反应
   排查：
   • 检查波特率：9600，正确
   • 检查接线：发现TX/RX接反
   解决：交换PA9和PA10的接线
   经验：串口通信必须交叉连接

2. 蜂鸣器不响（2026-06-18）
   现象：代码调用Buzzer_Beep()但不响
   排查：
   • 检查代码：逻辑正确
   • 检查硬件：发现面包板接触不良
   解决：重新插拔，确保接触良好

3. OLED字体异常（2026-06-18）
   现象：显示的文字方向混乱
   原因：字体方向设置错误
   解决：使用6x8字体，正确设置方向

4. 电机方向问题（2026-06-19）
   现象：右前电机方向相反
   原因：镜像安装
   解决：交换接线

【调试方法总结】
• 最小化测试：只保留一个功能
• LED/OLED指示：可视化调试
• 蜂鸣器反馈：声音确认
• 串口助手：数据抓包分析
''')

    # 问题7：创新点
    doc.add_heading('问题7：项目的创新点是什么？', level=2)
    doc.add_paragraph('''
【回答】

1. 协议设计创新：
   • 从5字节帧协议简化为单字节ASCII
   • 降低通信复杂度，提高可靠性
   • 便于调试和扩展

2. 接线方案创新：
   • 解决镜像电机方向问题
   • 通过交换接线实现统一控制
   • 简化软件逻辑

3. 调试方法创新：
   • OLED实时显示调试信息
   • 蜂鸣器声音反馈
   • 多层次测试方法

4. 模块化设计：
   • 清晰的代码结构
   • 独立的驱动模块
   • 易于维护和扩展

5. 实用性：
   • 成本低廉（<100元）
   • 易于复制
   • 适合教学演示
''')

    # 五、附录
    doc.add_heading('五、附录', level=1)

    doc.add_heading('A. 蓝牙协议速查表', level=2)
    protocol_table = doc.add_table(rows=9, cols=3)
    protocol_table.style = 'Table Grid'

    protocol_data = [
        ('字符', 'HEX', '功能'),
        ('0', '0x30', '停止'),
        ('1', '0x31', '前进'),
        ('2', '0x32', '后退'),
        ('3', '0x33', '左转'),
        ('4', '0x34', '右转'),
        ('5', '0x35', '加速'),
        ('6', '0x36', '减速'),
        ('7', '0x37', '蜂鸣器'),
    ]

    for i, (c, h, f) in enumerate(protocol_data):
        protocol_table.rows[i].cells[0].text = c
        protocol_table.rows[i].cells[1].text = h
        protocol_table.rows[i].cells[2].text = f

    doc.add_paragraph('')

    doc.add_heading('B. 硬件接线检查清单', level=2)
    doc.add_paragraph('''
【检查清单】

□ 电源系统
  □ 6节电池串联（9V）
  □ 电池+ → L298N的12V
  □ 电池- → L298N的GND
  □ L298N的5V → STM32的5V
  □ 所有模块共地

□ 蓝牙模块
  □ VCC → 3.3V（不是5V）
  □ GND → 共地
  □ TXD → PA10（RX）
  □ RXD → PA9（TX）

□ OLED显示
  □ VCC → 3.3V
  □ GND → 共地
  □ SCL → PB6
  □ SDA → PB7

□ 电机驱动（模块1-左侧）
  □ 12V → 电池+
  □ GND → 电池-
  □ ENA → PA0
  □ IN1 → PA4
  □ IN2 → PA5
  □ OUT1/OUT2 → 左前电机
  □ OUT3/OUT4 → 左后电机

□ 电机驱动（模块2-右侧）
  □ 12V → 电池+
  □ GND → 电池-
  □ ENA → PA1
  □ IN1 → PA6
  □ IN2 → PA7
  □ OUT1 → 右前电机-（交换）
  □ OUT2 → 右前电机+（交换）
  □ OUT3/OUT4 → 右后电机

□ 蜂鸣器
  □ VCC → 3.3V或5V
  □ GND → 共地
  □ I/O → PB0
''')

    # 保存文档
    output_path = r'D:\stmf132\蓝牙小车答辩问题与详细解答.docx'
    doc.save(output_path)
    print(f'文档已生成: {output_path}')

if __name__ == '__main__':
    create_document()
