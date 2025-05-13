/*
 * GpioHelper.h
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#pragma once

#include <cstring>
#include <functional>
#include <string>

class GpioHelper
{
  public:
	using PinChangeCallback = std::function<void(int pin, int value)>;

	static int togglePin(int pin);
	static int setPinValue(int pin, int value);
	static int getPinValue(int pin);
	static int pinNameToNumber(const std::string& pinName);
	static int monitorPin(int pin, PinChangeCallback callback);
	static void stopMonitoring(int pin);

  private:
	static int getPortOffset(char port);
};
