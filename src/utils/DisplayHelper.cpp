/*
 * DisplayHelper.cpp
 *
 *  Created on: 2025-02-26
 *      Author: Andy Everitt
 */

#include "DisplayHelper.h"
#include "Debug.h"
#include "utils/StorageHelper.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

#if T113
// Defined in buildroot-duetscreen/output/build/linux-.../include/video/sunxi_display2.h
#  define DISP_LCD_SET_BRIGHTNESS 0x102
#  define DISP_LCD_GET_BRIGHTNESS 0x103
#  define DISP_LCD_BACKLIGHT_ENABLE 0x104
#  define DISP_LCD_BACKLIGHT_DISABLE 0x105
#endif

// Define a structure to hold parameters for brightness operations.
// The actual structure may differ based on the driver's header files.
struct BrightnessParam
{
	unsigned int screen;	 // Which screen (0 or 1)
	unsigned int brightness; // Brightness value [0-255]
};

// Constructor: Opens the display device.
DisplayHelper::DisplayHelper(const char* device, unsigned int screen)
	: m_screen(screen)
{
#if T113
	m_fd = open(device, O_RDWR);
	if (m_fd < 0)
	{
		throw std::runtime_error(std::string("Failed to open device: ") + strerror(errno));
	}
#endif
}

// Destructor: Closes the device.
DisplayHelper::~DisplayHelper()
{
	if (m_fd >= 0)
	{
		close(m_fd);
	}
}

DisplayHelper& DisplayHelper::instance()
{
	static DisplayHelper instance;
	return instance;
}

// Sets the display brightness.
// brightness should be in the range [0, 100]
bool DisplayHelper::setBrightness(unsigned int percentage)
{
	auto& disp = instance();
	BrightnessParam param;
	if (percentage > 100)
	{
		percentage = 100;
	}
	// Scale brightness from 0-100 to 0-255 for the hardware
	uint8_t brightness = (percentage * 255) / 100;
	param.screen = disp.m_screen;
	param.brightness = brightness;
#if T113
	if (ioctl(disp.m_fd, DISP_LCD_SET_BRIGHTNESS, &param) < 0)
	{
		error("ioctl setBrightness failed");
		return false;
	}
#endif
	StorageHelper::setData(ID_SYS_BRIGHTNESS_KEY, percentage);
	return true;
}

// Gets the current display brightness.
// Returns a brightness value in the range [0, 100].
unsigned int DisplayHelper::getBrightness()
{
	auto& disp = instance();
	BrightnessParam param;
	param.screen = disp.m_screen;
#if T113
	if (ioctl(disp.m_fd, DISP_LCD_GET_BRIGHTNESS, &param) < 0)
	{
		perror("ioctl getBrightness failed");
		return 0;
	}
#endif
	return param.brightness;
}
