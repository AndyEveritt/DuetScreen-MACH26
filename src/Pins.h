/*
 * Pins.h
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#pragma once

#if T113
constexpr int GPIO_USB_DEVICE_DETECT = 192; // PG0
constexpr int GPIO_USB_SELECT = 193; // PG1
constexpr int GPIO_USB_STATE = 194;	 // PG2
#else
constexpr int GPIO_USB_DEVICE_DETECT = 0;
constexpr int GPIO_USB_SELECT = 0;
constexpr int GPIO_USB_STATE = 0;
#endif