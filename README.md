# PaperTracker

<div align="center">

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Status: Alpha](https://img.shields.io/badge/Status-Alpha-yellow.svg)]()

</div>


[English](README.md) | [中文](README_zh.md)


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
