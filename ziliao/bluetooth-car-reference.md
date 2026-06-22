# 蓝牙小车参考项目

来源：https://github.com/daohewang/Bluetooth-intelligent-car

## 核心设计思路

### 1. 蓝牙协议（极简单字节协议）

- **无帧格式**，无校验
- 直接发送ASCII字符：'0'-'6'
- 每个字符对应一个动作

| 字符 | 功能 |
|------|------|
| '0' | 停止 |
| '1' | 左转 |
| '2' | 右转 |
| '3' | 后退 |
| '4' | 前进 |
| '5' | 左旋转 |
| '6' | 右旋转 |

### 2. 蓝牙接收（中断直判）

```c
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        i = USART_ReceiveData(USART1);

        if(i == '0') flag = 0;
        if(i == '1') flag = 1;
        // ...
    }
}
```

### 3. 主程序（switch-case调度）

```c
while(1)
{
    switch(flag)
    {
        case 0: Stop(); break;
        case 1: Turnleft(); break;
        // ...
    }
}
```

## 关键启示

1. **协议越简单越好调试** - 先用单字节协议调通蓝牙通信
2. **中断中只做最少的事** - 只设置flag，不做复杂处理
3. **主循环负责所有业务逻辑** - 电机控制、OLED显示都在主循环
4. **APP发送ASCII字符比HEX更直观** - 测试更方便
