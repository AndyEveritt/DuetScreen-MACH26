/*
 * GpioHelper.cpp
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#include "GpioHelper.h"
#include "Debug.h"
#if T113
#  include <gpiod.h>
#endif

int GpioHelper::togglePin(int pin)
{
#if T113
	const char* chip_path = "/dev/gpiochip0";
	struct gpiod_chip* chip;
	struct gpiod_line* line;
	int value;

	chip = gpiod_chip_open(chip_path);
	if (!chip)
	{
		LOG_ERROR("Failed to open GPIO chip");
		return -1;
	}

	line = gpiod_chip_get_line(chip, pin);
	if (!line)
	{
		LOG_ERROR("Failed to get GPIO line");
		gpiod_chip_close(chip);
		return -1;
	}

	if (gpiod_line_request_output(line, "togglePin", 0) < 0)
	{
		LOG_ERROR("Failed to request GPIO line as output");
		gpiod_line_release(line);
		gpiod_chip_close(chip);
		return -1;
	}

	value = gpiod_line_get_value(line);
	gpiod_line_set_value(line, !value);

	gpiod_line_release(line);
	gpiod_chip_close(chip);
#endif
	return 0;
}

int GpioHelper::setPinValue(int pin, int value)
{
#if T113
	const char* chip_path = "/dev/gpiochip0";
	struct gpiod_chip* chip;
	struct gpiod_line* line;

	chip = gpiod_chip_open(chip_path);
	if (!chip)
	{
		LOG_ERROR("Failed to open GPIO chip");
		return -1;
	}

	line = gpiod_chip_get_line(chip, pin);
	if (!line)
	{
		LOG_ERROR("Failed to get GPIO line");
		gpiod_chip_close(chip);
		return -1;
	}

	if (gpiod_line_request_output(line, "setPinValue", value) < 0)
	{
		LOG_ERROR("Failed to request GPIO line as output");
		gpiod_line_release(line);
		gpiod_chip_close(chip);
		return -1;
	}

	gpiod_line_set_value(line, value);

	gpiod_line_release(line);
	gpiod_chip_close(chip);
#endif
	return 0;
}

int GpioHelper::getPinValue(int pin)
{
#if T113
	const char* chip_path = "/dev/gpiochip0";
	struct gpiod_chip* chip;
	struct gpiod_line* line;
	int value;

	chip = gpiod_chip_open(chip_path);
	if (!chip)
	{
		LOG_ERROR("Failed to open GPIO chip");
		return -1;
	}

	line = gpiod_chip_get_line(chip, pin);
	if (!line)
	{
		LOG_ERROR("Failed to get GPIO line");
		gpiod_chip_close(chip);
		return -1;
	}

	if (gpiod_line_request_input(line, "getPinValue") < 0)
	{
		LOG_ERROR("Failed to request GPIO line as input");
		gpiod_line_release(line);
		gpiod_chip_close(chip);
		return -1;
	}

	value = gpiod_line_get_value(line);

	gpiod_line_release(line);
	gpiod_chip_close(chip);
	return value;
#endif
	return 0;
}

int GpioHelper::getPortOffset(char port)
{
	return (port - 'A') * 32;
}

int GpioHelper::pinNameToNumber(const std::string& pinName)
{
	if (pinName.empty())
	{
		LOG_ERROR("Empty pin name");
		return -1;
	}
	if (pinName[0] != 'P' && pinName[0] != 'p')
	{
		LOG_ERROR("Invalid pin name: {:s}", pinName);
		return -1;
	}
	if (pinName.length() < 3)
	{
		LOG_ERROR("Invalid pin name length: {:s}", pinName);
		return -1;
	}
	char port = pinName[1];
	if (port < 'B' || port > 'G')
	{
		LOG_ERROR("Invalid port: {:s}", pinName);
		return -1;
	}
	int pin = std::stoi(pinName.substr(2));
	if (pin < 0 || pin > 31)
	{
		LOG_ERROR("Invalid pin number: {:d}", pin);
		return -1; // Assuming pin numbers are 0-31
	}
	return getPortOffset(port) + pin;
}