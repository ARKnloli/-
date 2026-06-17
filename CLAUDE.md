# STM32F103C8T6 智能小车项目指引

## 项目概述

本项目基于STM32F103C8T6微控制器开发一个智能小车系统，支持蓝牙遥控、OLED状态显示、蜂鸣器提示等功能。

## 开发环境

- **IDE**: Keil MDK-ARM v5 或 STM32CubeIDE
- **编译器**: ARM Compiler v6 (armclang)
- **调试器**: ST-Link V2
- **串口调试**: SSCOM32 或 PuTTY

## 目录结构

```
stmf132/
├── lanyaxc/                    # CubeMX生成的工程（主工程目录）
│   ├── Core/                   # 核心代码
│   │   ├── Src/                # 源文件
│   │   │   ├── main.c          # 主程序
│   │   │   ├── gpio.c          # GPIO配置
│   │   │   ├── i2c.c           # I2C配置
│   │   │   ├── tim.c           # 定时器配置
│   │   │   └── usart.c         # 串口配置
│   │   └── Inc/                # 头文件
│   ├── Drivers/                # HAL驱动库
│   └── MDK-ARM/                # Keil工程文件
├── docs/                       # 项目文档
│   ├── requirements/           # 需求文档
│   ├── technical/              # 技术规范
│   ├── design/                 # 设计文档
│   └── execution/              # 执行步骤
├── devlog/                     # 开发日志（按年/月/日记录）
└── tools/                      # 工具脚本
```

## CubeMX工程配置

### 已配置的外设
- **USART1**: PA9(TX), PA10(RX) - 蓝牙通信，波特率115200
- **I2C1**: PB6(SCL), PB7(SDA) - OLED显示
- **TIM2_CH1**: PA0 - 左电机PWM
- **TIM2_CH2**: PA1 - 右电机PWM

### 待配置（需手动添加）
- **GPIO**: PA4-PA7 - 电机方向控制
- **GPIO**: PB0 - 蜂鸣器（低电平触发）

## 文档指引

### 需求文档
- `docs/requirements/project-requirements.md` - 项目总体需求
- `docs/requirements/hardware-requirements.md` - 硬件需求清单
- `docs/requirements/software-requirements.md` - 软件功能需求

### 技术规范
- `docs/technical/coding-standard.md` - 代码编写规范
- `docs/technical/hardware-interface.md` - 硬件接口规范
- `docs/technical/testing-standard.md` - 测试规范

### 设计文档
- `docs/design/architecture.md` - 系统架构设计
- `docs/design/hardware-design.md` - 硬件设计（接线图、原理图）
- `docs/design/software-design.md` - 软件设计（流程图、状态机）

### 执行步骤
- `docs/execution/development-plan.md` - 开发计划
- `docs/execution/daily-workflow.md` - 每日工作流程
- `docs/execution/delivery-checklist.md` - 交付检查清单

## 开发流程

### 每日工作流程

1. **开始工作前**
   - 阅读 `devlog/` 中最近的日志
   - 确认今日待办事项
   - 检查硬件连接状态

2. **开发过程中**
   - 遵循 `docs/technical/coding-standard.md` 编写代码
   - 每完成一个功能模块，进行单元测试
   - 遇到问题及时记录到日志

3. **结束工作时**
   - 更新 `devlog/YYYY/MM-DD.md`
   - 记录完成事项、遇到问题、明日计划
   - 提交代码（如使用Git）

### 开发阶段

| 阶段 | 内容 | 预计时间 |
|------|------|----------|
| 第一阶段 | 环境搭建 + 基础驱动 | 2-3天 |
| 第二阶段 | 功能模块开发 | 3-4天 |
| 第三阶段 | 应用层集成 | 2-3天 |
| 第四阶段 | 测试优化 | 2-3天 |

## 编码规范摘要

- 使用STM32CubeMX生成的HAL库代码
- 用户代码写在 `/* USER CODE BEGIN */` 和 `/* USER CODE END */` 之间
- 函数命名：`模块名_功能名`，如 `Motor_SetSpeed()`
- 变量命名：小写下划线，如 `motor_speed`
- 常量/宏命名：大写下划线，如 `LEFT_MOTOR_FWD_PIN`
- 头文件引用：使用 `#include "模块名.h"` 格式

## 硬件接线原则

1. **电源安全**：先接GND，再接VCC
2. **信号线**：最后连接，确保电平匹配
3. **电机驱动**：L298N独立供电，逻辑电平3.3V兼容
4. **蓝牙模块**：HC-05/06使用3.3V供电，TX/RX交叉连接

## 常用调试命令

```bash
# 编译（Keil）
UV4.exe -b Project.uvprojx -o build.log

# 烧录（ST-Link）
ST-LINK_CLI.exe -c SWD -w firmware.bin 0x08000000 -Rst

# 串口监控
putty -serial COM3 -sercfg 115200,8,n,1,N
```

## 注意事项

1. **电源管理**
   - STM32工作电压：2.0V ~ 3.6V
   - L298N电机驱动：5V ~ 35V
   - 蓝牙模块：3.3V（不要接5V！）

2. **I2C总线**
   - 需要4.7kΩ上拉电阻到VCC
   - OLED和MPU6050地址不要冲突

3. **电机控制**
   - PWM频率建议1kHz ~ 20kHz
   - 启动时占空比从0逐渐增加

4. **蓝牙通信**
   - 波特率默认9600或38400
   - 数据格式：帧头 + 命令 + 参数 + 校验

## 相关资源

- [STM32F103C8T6数据手册](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [标准外设库文档](https://www.st.com/resource/en/user_manual/um0427.pdf)
- [L298N数据手册](https://www.st.com/resource/en/datasheet/l298.pdf)

---

## 项目需求与评分标准

### 基本功能（80分）

| 序号 | 功能 | 分值 | 状态 |
|------|------|------|------|
| 1 | 小车通过蓝牙模块接收手机发送的指令 | 30分 | ✅ 已实现 |
| 2 | 根据指令执行前进、左转、右转、后退、停止 | 30分 | ✅ 已实现 |
| 3 | 为小车操作播放不同的提示音 | 20分 | ✅ 已实现 |

### 扩展功能（15分）

| 序号 | 功能 | 分值 | 状态 |
|------|------|------|------|
| 1 | 通过蓝牙模块实现小车速度调整 | 15分 | ✅ 已实现 |

### 机动分数（5分）

根据作品完成情况进行评分。

**总分：100分**

---

## 硬件设计注意事项

### 1. 去耦电容

| 位置 | 电容值 | 作用 |
|------|--------|------|
| STM32 每个VDD引脚 | 100nF (0.1uF) | 高频去耦 |
| STM32 VDDA | 100nF + 10uF | 模拟电源滤波 |
| LM2596 输入端 | 220uF | 输入滤波 |
| LM2596 输出端 | 100uF | 输出滤波 |
| L298N 5V输出 | 100uF | 逻辑电源滤波 |
| 蓝牙 VCC | 100nF | 电源去耦 |
| OLED VCC | 100nF | 电源去耦 |

### 2. 电机瞬时电流保护

- **LM2596** 最大输出电流3A，电机启动电流可能超过此值
- **解决方案**：
  1. 电机缓启动（软件实现，占空比渐增）
  2. L298N独立供电，不经过LM2596
  3. 在电机电源端加220uF电解电容

### 3. 电源拓扑

```
电池(6V) ──┬──→ L298N 12V输入（电机专用）
            │
            └──→ LM2596 IN → 5V OUT ──→ STM32 5V
                                           │
                                      板载3.3V ──→ 蓝牙/OLED/蜂鸣器
```

### 4. 软件保护策略

- **电机缓启动**：PWM占空比从0渐增到目标值
- **蓝牙与电机时序**：接收指令后先停止，再执行新动作
- **蜂鸣器独立**：操作提示音不阻塞主循环

---

## 已实现的功能

| 模块 | 功能 | 文件 |
|------|------|------|
| 电机控制 | 前进/后退/左转/右转/停止/调速 | motor_control.c |
| 蜂鸣器 | 操作提示音/成功/错误提示 | buzzer.c |
| 蓝牙通信 | 协议解析/命令分发/回调机制 | bluetooth.c |
| OLED显示 | 方向/速度/进度条显示 | oled.c | 