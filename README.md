# DuetScreen

This project is a GUI for the Duet3D screen. It is based on the LittlevGL (LVGL) library. The program can be compiled to run on the Duet3D screen or on a PC. The PC version is useful for development and testing purposes.

## Getting started

### Flashing a new Duet3D screen
1. Download the latest sdcard image from the release page.
2. Flash a microSD card with the image
    - use [balenaEtcher](https://www.balena.io/etcher/) on Windows
    - use `dd` on Linux
        - ```bash
            sudo dd if=duet-screen-*.img of=/dev/sdX bs=4M
            ```
3. Insert the microSD card into the Duet3D screen and power it on.


### Connecting the Duet3D screen to a WiFi network
There are a few methods to connect the Duet3D screen to a WiFi network. The recommended method is to use the `wpa_supplicant.conf` file. This file should be placed on the microSD card after it has been flashed.

1. You can copy a file called `wpa_supplicant.conf` to the root of the microSD card. This file should contain the WiFi credentials in the following format:
    ```
    ctrl_interface=/var/run/wpa_supplicant
    update_config=1
    ap_scan=1

    network={
        ssid="your-SSID"
        psk="your-PASSWORD"
        key_mgmt=WPA-PSK
    }
    ```
    - This method is the easiest if you are setting up multiple screens, or you know the WiFi credentials in advance.
2. Alternatively, you can connect to a network using the GUI.
    - This method is useful if you are setting up a single screen and you do not know the WiFi credentials in advance.
    - The GUI is currently placeholder and has some known issues.


## Connecting to a Duet3D mainboard
Multiple methods are available to connect the Duet3D screen to a mainboard. The recommended method is to use a USB cable. This allows for the best performance and is the easiest to set up.

### USB
1. Connect the Duet3D screen to the mainboard using a USB cable.
    - Both the USB-A and USB-C ports on the screen can be used.
    - If using the USB-C port, make sure to set the screen to USB host mode.
2. In the GUI, select the USB connection method.

### WiFi
1. Ensure the Duet3D screen is connected to the same WiFi network as the mainboard.
    - See the [Connecting the Duet3D screen to a WiFi network](#connecting-the-duet3d-screen-to-a-wifi-network) section above.
2. In the GUI, select the WiFi connection method.
3. Enter the IP address of the mainboard.

### UART

> [!WARNING]
> This method is for legacy support only to provide an easy upgrade path for PanelDue users. It is not recommended for new installations.

1. Connect the Duet3D screen to the mainboard using a UART cable.
    - Use connector `U5` on the screen.
2. In the GUI, select the UART connection method.
3. Set the baud rate on the mainboard to `115200`.


## Updating the Duet3D screen
Several methods are available to update the Duet3D screen.

1. **Using the GUI**
    - Copy the update file (`duetscreen.tar.gz`) to the root directory of a USB flash drive.
    - Insert the USB flash drive into the Duet3D screen.
    - In the GUI you will be prompted to update the screen.

2. **Force Update**
    - If the GUI is not working, you can force an update by renaming the update file to `update.tar.gz` and placing it in the root directory of the flash drive or microSD card.
    - Insert the USB flash drive or microSD card into the Duet3D screen.
    - *(If using a microSD card)* Power on the screen and it will automatically update.

Occasionally, an update may require the whole microSD card to be reflashed. This will be indicated in the release notes. In this case, follow the instructions in the [Flashing a new Duet3D screen](#flashing-a-new-duet3d-screen) section above.


