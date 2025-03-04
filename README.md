# PaperTracker

<div align="center">

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Status: Alpha](https://img.shields.io/badge/Status-Alpha-yellow.svg)]()

</div>

## 项目简介

PaperTracker 是一款高效的面部追踪应用程序，能够捕获和分析面部表情，并通过 OSC 协议实时传输数据。本项目使用 C++ 开发，显著降低了 CPU 占用率，提供流畅的使用体验。

> **注意**：本项目目前处于 Alpha 开发阶段，文档可能不完善，并且可能存在潜在问题。欢迎提交反馈和建议！

## 配网指南
https://fcnk6r4c64fa.feishu.cn/wiki/B4pNwz2avi4kNqkVNw9cjRRXnth?from=from_copylink

## 主要功能

- 🎥 支持 ESP32 摄像头连接
- 👁️ 实时面部表情识别和分析
- 📡 通过 OSC 协议向其他应用程序传输数据
- 📱 简便的 WiFi 设置界面和固件更新功能
- 🔍 自定义区域选择和高级图像处理

## 系统要求

- Windows 操作系统
- ESP32-S3 设备和摄像头（需要烧录 [face_tracker](https://github.com/paper-tei/face_tracker) 对应固件）
- 或者使用本地摄像头

## 快速开始

### 安装

1. 从 [Releases](../../releases) 页面下载最新版本
2. 解压安装包到本地文件夹
3. 运行 PaperTracker.exe

### 基本使用

1. 连接 ESP32 设备或本地摄像头
2. 首次使用时配置 WiFi 参数（ESP32 模式下）
3. 使用界面调整亮度和感兴趣区域(ROI)
4. 开始捕获和分析面部表情
5. 查看日志窗口获取运行状态信息

## 开发与构建

```bash
# 克隆仓库
git clone https://github.com/yourusername/PaperTracker.git

# 创建构建目录
mkdir build && cd build

# 配置并构建项目
cmake ..
cmake --build .
```

## 许可证

本项目采用 [GNU 通用公共许可证 v3.0](LICENSE) 进行许可。

### 许可要求

- **商业使用**：允许商业使用，但必须遵循 GPL 许可证的所有条款。
- **源代码要求**：任何使用、修改或分发本软件的个人或组织，包括将软件集成到硬件产品中的厂商，必须：
  1. 公开其修改后的完整源代码
  2. 以相同的 GPL 许可证发布
  3. 明确注明 PaperTracker 的原始版权和来源

## 第三方组件

本项目使用了来自 [Project Babble v2.0.5](https://github.com/Project-Babble/ProjectBabble) 的代码和模型，这些组件根据 Apache License 2.0 许可发布。详见 [THIRD-PARTY.txt](THIRD-PARTY.txt)。

## 贡献

欢迎通过 Issue 和 Pull Request 参与项目开发。在提交代码前，请确保您了解并同意遵守本项目的许可条款。

## 联系方式

- 问题反馈：请使用 [GitHub Issues](../../issues)

---

# PaperTracker (English)

<div align="center">

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Status: Alpha](https://img.shields.io/badge/Status-Alpha-yellow.svg)]()

</div>

## Overview

PaperTracker is an efficient facial tracking application that captures and analyzes facial expressions, transmitting data via the OSC protocol. Developed in C++, it significantly reduces CPU usage for a smooth user experience.

> **Note**: This project is currently in Alpha development stage. Documentation may be incomplete, and potential issues might exist. Feedback and suggestions are welcome!

## Key Features

- 🎥 Support for both ESP32 camera and local camera connections
- 👁️ Real-time facial expression recognition and analysis
- 📡 Data transmission to other applications via OSC protocol
- 📱 Easy WiFi setup interface and firmware update functionality
- 🔍 Custom region selection and advanced image processing

## System Requirements

- Windows operating system
- ESP32-S3 device and camera (requires [face_tracker](https://github.com/paper-tei/face_tracker) firmware)
- Or a local camera

## Quick Start

### Installation

1. Download the latest version from the [Releases](../../releases) page
2. Extract the package to a local folder
3. Run PaperTracker.exe

### Basic Usage

1. Connect your ESP32 device or local camera
2. Configure WiFi parameters for first-time use (ESP32 mode)
3. Adjust brightness and Region of Interest (ROI) using the interface
4. Begin capturing and analyzing facial expressions
5. Check the log window for status information

## Development and Building

```bash
# Clone the repository
git clone https://github.com/yourusername/PaperTracker.git

# Create build directory
mkdir build && cd build

# Configure and build the project
cmake ..
cmake --build .
```

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE).

### License Requirements

- **Commercial Use**: Commercial use is permitted, subject to compliance with all GPL license terms.
- **Source Code Requirements**: Any individual or organization using, modifying, or distributing this software, including hardware manufacturers integrating it into their products, must:
  1. Make the complete modified source code publicly available
  2. License their modifications under the same GPL license
  3. Clearly attribute the original PaperTracker copyright and source

## Third-Party Components

This project uses code and models from [Project Babble v2.0.5](https://github.com/Project-Babble/ProjectBabble), which are licensed under the Apache License 2.0. See [THIRD-PARTY.txt](THIRD-PARTY.txt) for details.

## Contributing

Contributions via Issues and Pull Requests are welcome. Before submitting code, please ensure you understand and agree to comply with the project's license terms.

## Contact

- For issue reporting: Please use [GitHub Issues](../../issues)
