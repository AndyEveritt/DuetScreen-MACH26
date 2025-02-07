# DuetScreen

This project is a GUI for the Duet3D screen. It is based on the LittlevGL (LVGL) library. The program can be compiled to run on the Duet3D screen or on a PC. The PC version is useful for development and testing purposes.

## Get started

Clone the project and the related sub modules:

```bash
git clone --recursive https://github.com/Duet3D/DuetScreen.git
```

### Linux

Copy below in the Terminal:
For Ubuntu

```bash
sudo apt-get update && sudo apt-get install -y build-essential libsdl2-dev cmake ninja-build libusb-1.0-0-dev pkg-config
```

For instructions to run the simulator on Linux/WSL2, see [DEVELOPMENT.md](DEVELOPMENT.md).

## Usage
### Visual Studio Code

The project is configured for [VSCode](https://code.visualstudio.com) and is tested on:
- WSL2 (Ubuntu Linux)

### Build

To build the project, run the following commands:

```bash
cmake --preset {PresetName}
cmake --build --preset {PresetName}
```

Valid preset names can be found by running:

```bash
cmake --list-presets
```
