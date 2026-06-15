# 技术规范 — 历史粘贴板

## 技术栈

| 层面 | 技术 |
|------|------|
| 桌面框架 | PyQt6（Qt 6.x Python 绑定） |
| 数据库 | Python 内置 sqlite3 |
| UI | Qt Widgets |
| 打包 | PyInstaller（单文件 exe） |
| 运行环境 | Python 3.13+ |

> **选型原因**：Electron 在 Windows 上存在已知 Bug（GitHub #49034），`require('electron')` 返回字符串而非 API 对象，无法正常使用。PyQt6 在 Windows 上稳定可靠，系统托盘、剪贴板、SQLite 全部内置支持。

## 项目结构

```
d:\liantueban\
├── CLAUDE.md
├── requirements.txt
├── main.py               # 应用入口（初始化、托盘、启动）
├── database.py           # 数据库初始化 + CRUD + 清理
├── clipboard_monitor.py  # 剪贴板监听线程
├── tray_manager.py       # 系统托盘管理
├── ui/
│   ├── main_window.py    # 主窗口（卡片列表、搜索栏、工具栏）
│   ├── card_widget.py    # 单条记录卡片组件
│   └── settings_panel.py # 设置面板（存储期限、容量、自启）
├── assets/
│   └── icon.png          # 托盘图标 (32×32)
├── images/               # 图片存储（运行时自动创建）
├── docs/                 # 项目文档
└── logs/                 # 开发日志
```

## 应用入口 (main.py)

### 模块职责

- **QApplication 初始化**：创建 Qt 应用实例
- **数据库初始化**：调用 database.init_db()
- **系统托盘**：创建 TrayManager 实例，图标 + 右键菜单
- **剪贴板监听**：启动 ClipboardMonitor 线程
- **主窗口**：创建 MainWindow，默认隐藏
- **开机自启**：Windows 注册表操作
- **清理调度**：QTimer 每 30 分钟执行清理

### 应用生命周期

```
启动 → 初始化DB → 创建托盘 → 启动剪贴板监听 → 等待用户操作
关闭窗口 → 隐藏到托盘（不退出）
托盘菜单"退出" → 停止监听 → 退出应用
```

## 剪贴板监听 (clipboard_monitor.py)

### 监听策略

```
QThread 子线程，每 500ms 轮询:
  - QApplication.clipboard().text()
    如果与上次不同 → 写入数据库，发射信号通知UI
  - QApplication.clipboard().image()
    如果与上次不同 → 保存为 PNG，记录路径到数据库
```

去重规则：
- 文字：直接字符串比较
- 图片：对 QImage 做 SHA256 hash 比较

## 数据库 (database.py)

### 表结构

```sql
CREATE TABLE IF NOT EXISTS items (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  type TEXT NOT NULL CHECK(type IN ('text','image')),
  content TEXT,
  size INTEGER DEFAULT 0,
  pinned INTEGER DEFAULT 0 CHECK(pinned IN (0,1)),
  created_at TEXT NOT NULL DEFAULT (datetime('now','localtime'))
);

CREATE TABLE IF NOT EXISTS settings (
  key TEXT PRIMARY KEY,
  value TEXT NOT NULL
);
```

### 暴露的函数

| 函数 | 说明 |
|------|------|
| `init_db()` | 建表 + 初始化默认设置 |
| `add_item(type, content, size)` | 插入记录，返回 id |
| `get_items(search=None, limit=1000)` | 查询列表（搜索过滤、置顶优先+时间降序） |
| `pin_item(id, pinned)` | 更新置顶状态 |
| `delete_item(id)` | 删除记录（图片同步删除文件） |
| `get_settings()` | 获取全部设置，返回 dict |
| `set_setting(key, value)` | 更新单项设置 |
| `cleanup_by_age(retention_days)` | 按天数清理过期记录 |
| `cleanup_by_size(max_bytes)` | 按容量清理图片文件 |
| `get_last_text()` | 获取最后一条文字内容 |
| `get_last_image_hash()` | 获取最后图片 hash |

## UI 组件

| 组件 | 文件 | 说明 |
|------|------|------|
| MainWindow | ui/main_window.py | QWidget 主窗口，包含搜索栏、卡片滚动区、底部状态栏 |
| CardWidget | ui/card_widget.py | 单条记录卡片，支持点击复制、置顶、删除 |
| SettingsPanel | ui/settings_panel.py | QWidget 滑入面板，存储期限/容量/自启设置 |

### 信号与槽

```
ClipboardMonitor.new_item_signal  →  MainWindow.refresh_list()
CardWidget.clicked                →  MainWindow.copy_item()
CardWidget.pin_toggled            →  database.pin_item()
CardWidget.delete_requested       →  database.delete_item()
TrayManager.show_requested        →  MainWindow.show()
TrayManager.quit_requested        →  QApplication.quit()
```

## 打包配置

PyInstaller，目标格式：**单文件 exe**

- 入口：main.py
- 图标：assets/icon.png
- 隐藏控制台窗口（--noconsole）
- 添加数据文件：assets/
