/*
 * DisplayHelper.h
 *
 *  Created on: 2025-02-26
 *      Author: Andy Everitt
 */

#pragma once

class DisplayHelper
{
  public:
	// Sets the display brightness.
	// brightness should be in the range [0, 100]
	static bool setBrightness(unsigned int percentage);
	static bool setScreenSaverBrightness(unsigned int percentage);

	static void enableScreenSaver(bool enable);

	// Gets the current display brightness.
	// Returns a brightness value in the range [0, 100].
	static unsigned int getBrightness();

  private:
	DisplayHelper(const char* device = "/dev/disp", unsigned int screen = 0);
	~DisplayHelper();

	static DisplayHelper& instance();

	bool setBrightnessInner(unsigned int percentage);

	int m_fd = -1;		   // File descriptor for the display device
	unsigned int m_screen; // Screen number (typically 0 or 1)
	unsigned int m_percentage = 100;
	unsigned int m_screensaverPercentage = 0;
	unsigned int m_currentBrightness = 0;
};
