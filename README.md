# MyClient


## 项目简介

MyClient 是一个基于 Qt 6.9.0 的跨平台桌面客户端，主要用于即时通讯、用户管理等场景。项目采用模块化设计，便于扩展和维护。

> ⚠️ 本项目的 qwindowkit 组件必须使用 Qt 6.9.0 编译，开发环境务必保持一致，否则会导致编译或运行异常。

## 功能特性

- 多用户聊天与好友管理
- 头像上传与切换
- 消息记录与数据库存储
- 网络通信（Socket 客户端）
- 丰富的 UI 组件与自定义样式
- 支持多平台（Windows/Linux，主开发环境为 Windows）

## 主要模块说明

- **app/**：主窗口、核心 UI 逻辑
- **DataBaseManage/**：数据库管理与数据模型
- **Network/**：网络通信、消息处理、服务接口
- **qwindowkit/**：窗口管理与跨平台适配（需 Qt 6.9.0）
- **resources/**：资源文件（图片、样式、配置、SVG 等）
- **utils/**：工具库、配置管理、滚动条等辅助功能
- **widgets/**：自定义控件（如头像、按钮、输入框等）

## 源码目录结构（简要）

- src/
  - app/           # 主窗口与界面逻辑
  - DataBaseManage/ # 数据库与数据模型
  - Network/        # 网络通信与服务
  - qwindowkit/     # 跨平台窗口管理（Qt 6.9.0）
  - resources/      # 图片、样式、配置等资源
  - utils/          # 工具类与通用功能
  - widgets/        # 自定义控件

其它目录：
- build/   # 构建产物
- docs/    # 文档说明
- ui/      # Qt Designer UI 文件

## 环境与依赖

- 操作系统：Windows（推荐），支持 Linux
- Qt 版本：**6.9.0**（必须）
- 编译工具：CMake、Ninja 或 Qt Creator
- 编译器：MinGW（与 Qt 版本匹配）


## 常见问题

- 问：使用 Qt 其他版本会怎样？
  - 答：qwindowkit 与部分模块已使用 Qt 6.9.0 编译，使用不同 Qt 主版本通常会产生 ABI 不兼容，出现编译或运行时错误。请务必使用 6.9.0。
- 问：找不到 qwindowkit 的库或头文件？
  - 答：检查 `src/qwindowkit/win` 或 `src/qwindowkit/linux` 下的 include/lib 是否存在，确保 CMakeLists.txt 中指向正确路径，或将其安装到系统 Qt 路径中。


## 在 Qt Creator 中打开（推荐）

1. 打开 Qt Creator -> 选择 `Open CMake Project` -> 选择仓库根目录下的 `CMakeLists.txt`。
2. 在配置对话框中选择 Kit：`Desktop Qt 6.9.0 MinGW 64-bit`（或你安装的 Qt 6.9.0 对应 kit）。
3. Configure / Run / Build。

## 快速开始

1. 安装 Qt 6.9.0 与 MinGW
2. 安装 CMake 与 Ninja
3. 配置环境变量（Qt、CMake、Ninja）
4. 命令行构建：

   ```powershell
   cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:\Qt\6.9.0\mingw\lib\cmake"
   cmake --build build --config Release
   ```

5. 或用 Qt Creator 打开 CMakeLists.txt，选择 Desktop Qt 6.9.0 MinGW 64-bit Kit，配置并编译。

## 贡献方式

欢迎通过 PR 或 issue 提交建议、修复 bug 或新增功能。

## License

本项目遵循 MIT 协议，详情见 LICENSE 文件。

---
此 README 自动生成，若有结构或功能变更请及时更新。