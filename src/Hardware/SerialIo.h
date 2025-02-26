/*
 * SerialIO.h
 *
 * Created: 04/02/2023
 *  Author: Loïc G.
 *
 *  This file is a modified version of the SerialIO.h source file from PanelDue
 *  The original can be found at https://github.com/Duet3D/PanelDueFirmware/blob/master/src/Hardware/SerialIo.h
 */

#ifndef JNI_SERIALIO_HPP_
#define JNI_SERIALIO_HPP_

#include "UartController.h"
#include <Duet3D/General/String.h>
#include <cstddef>
#include <memory>
#include <sys/types.h>

namespace SerialIo
{
	bool Init(const char* device, speed_t baudRate);
	void Shutdown();
	bool Send(const std::string& gcode);
	size_t Sendf(const char* fmt, ...) __attribute__((format(printf, 1, 0)));

	// New methods to control UART
	bool SetBaudRate(speed_t baudRate);
	bool IsConnected();
	void SetDataCallback(UartController::DataCallback callback);
} // namespace SerialIo

#endif /* JNI_SERIALIO_HPP_ */
