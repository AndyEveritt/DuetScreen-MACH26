/*
 * GpioHelper.h
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#pragma once

#include <cstring>
#include <string>

class GpioHelper
{
  public:
	static int togglePin(int pin);
    static int setPinValue(int pin, int value);
    static int getPinValue(int pin);
	static int pinNameToNumber(const std::string& pinName);

  private:
	static int getPortOffset(char port);
};

extern const int GPIO_USB_SELECT;
extern const int GPIO_USB_STATE;
