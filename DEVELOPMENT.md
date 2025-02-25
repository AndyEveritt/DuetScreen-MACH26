# Development

The following steps are required to setup VSCode as the development environment for the project.
- Copy `.vscode/settings.json.default` to `.vscode/settings.json`
- If you want to debug code running on the physical screen then you will need to:
  - Clone the [buildroot-duetscreen](https://github.com/Duet3D/buildroot-duetscreen) repository.
  - Build the [buildroot-duetscreen](https://github.com/Duet3D/buildroot-duetscreen) project.
  - Set the `buildroot_path` and `duetscreen_ip` settings in `.vscode/settings.json` to the correct values.

## Simulating
It is possible to simulate the GUI on PC without access to the physical hardware. This can be beneficial for testing and development purposes as it allows for debugging using gdb. 

The simulation is only setup to run on Linux or WSL2 on Windows.

The following steps are required to run the GUI on PC:

1. Install the required dependencies:
```bash
sudo apt-get update && sudo apt-get install -y build-essential libsdl2-dev cmake ninja-build libusb-1.0-0-dev pkg-config clang-format
```

2. Setup udev rules for USB communications
```bash
sudo bash -c 'cat ./config/99-usb.rules > /etc/udev/rules.d/99-usb.rules'
sudo service udev restart
sudo usermod -aG plugdev $USER
```

3. WSL2 only:
- Attach the Duet as a USB device using usbipd

4. Configure the project:
```bash
cmake --preset Simulation
```

5. Build the project:
```bash
cmake --build --preset Simulation
```

## Debugging
The program can be debugged using gdb when running as a simulation or on the physical hardware.

VSCode has been configured for both of these scenarios. The following steps are required to setup debugging in VSCode:

