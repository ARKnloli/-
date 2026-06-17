# CLAUDE.md — 历史粘贴板项目指引

## 项目概述

Windows 桌面粘贴板管理工具。自动记录复制内容（文字+图片），系统托盘常驻，支持搜索、置顶、删除。

## 规范文件路径

| 文档 | 路径 | 说明 |
|------|------|------|
| 产品需求 | [docs/requirements.md](docs/requirements.md) | 功能和 UI 需求定义 |
| 技术规范 | [docs/tech-spec.md](docs/tech-spec.md) | 技术栈、架构、数据库设计 |
| UI 设计规范 | [docs/design-spec.md](docs/design-spec.md) | 配色、布局、组件样式 |
| 执行计划 | [docs/execution-plan.md](docs/execution-plan.md) | 分阶段执行步骤和验证标准 |

## 开发日志

[logs/](logs/) — 每次开发会话结束后在此目录创建 `YYYY-MM-DD.md` 日志文件，记录：
- 当日完成事项
- 日终待办事项
- 遇到的问题和决策

## 工作约定

1. **分阶段推进**：严格按照 [执行计划](docs/execution-plan.md) 的阶段顺序，一阶段完成验证后再进入下一阶段
2. **先读规范再动手**：每次开发前先阅读对应的规范文档
3. **记录日志**：每次会话结束前写开发日志
4. **保持简洁**：代码不加多余注释，函数命名即文档
5. **不引入额外依赖**：非必要不加 pip 包
6. **本地优先**：所有数据只存本地，禁止网络请求
7. **中文交互**：UI 文案用中文，代码标识符用英文。与用户沟通全部使用中文，包括提问选项、确认提示等所有面向用户的文字

## 常用命令

```bash
pip install -r requirements.txt   # 安装依赖
python main.py                    # 启动应用（开发模式）
pyinstaller main.spec             # 打包为 exe
```
