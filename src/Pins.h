/*
 * Pins.h
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#pragma once

#if T113
constexpr int GPIO_USB_SELECT = 193; // PG1
constexpr int GPIO_USB_STATE = 194;	 // PG2
constexpr const char* USB_OTG_ROLE_PATH = "/sys/devices/platform/soc@3000000/soc@3000000:usbc0@0/otg_role";
#else
constexpr int GPIO_USB_SELECT = 0;
constexpr int GPIO_USB_STATE = 0;
constexpr const char* USB_OTG_ROLE_PATH = "/tmp/otg_role";
#endif