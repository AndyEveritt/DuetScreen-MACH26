/*
 * SerialIO.cpp
 *
 * Created: 04/02/2023
 *  Author: Loïc G.
 *
 *  This file is a modified version of the SerialIO.cpp source file from PanelDue
 *  The original can be found at https://github.com/Duet3D/PanelDueFirmware/blob/master/src/Hardware/SerialIo.cpp
 *
 */

#include "SerialIo.h"
#include "Comm/Communication.h"
#include "Comm/JsonDecoder.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include <array>
#include <string>

namespace SerialIo
{
	constexpr size_t UART_DATA_BUF_SIZE = 32 * 1024; // 32KB

	static std::unique_ptr<UartController> s_uart;
	static std::array<unsigned char, UART_DATA_BUF_SIZE> s_buffer;
	static size_t s_bufferLen = 0;

	static void processData(const std::string_view data);

	bool Init(const char* device, speed_t baudRate)
	{
#if SIMULATION
		LOG_INFO("Initializing simulated UART on device: {:s}", device);
#endif
		s_uart = std::make_unique<UartController>();

		if (!s_uart->setBaudRate(baudRate))
		{
			LOG_ERROR("Failed to set baud rate");
			return false;
		}
		RestoreDataCallback();

		return s_uart->open(device);
	}

	void Shutdown()
	{
		if (s_uart)
		{
			s_uart.reset();
		}
	}

	ssize_t Send(std::string_view data)
	{
		if (!s_uart)
		{
			LOG_ERROR("UART not initialized");
			return -1;
		}
		return s_uart->send(data);
	}

	bool SetBaudRate(speed_t baudRate)
	{
		return s_uart && s_uart->setBaudRate(baudRate);
	}

	bool IsConnected()
	{
		return s_uart && s_uart->isOpen();
	}

	void SetDataCallback(UartController::DataCallback callback)
	{
		if (s_uart)
		{
			s_uart->setReceiveCallback(callback);
		}
	}

	void RestoreDataCallback()
	{
		if (s_uart)
		{
			s_uart->setReceiveCallback(processData);
		}
	}

	static void processData(const std::string_view data)
	{
		size_t len = data.length();
		LOG_VERBOSE("Received ({:d}) '{:s}'", len, data);

		if (Comm::DUET.GetCommunicationType() != Comm::CommunicationType::uart)
		{
			s_bufferLen = 0;
			return;
		}

		if (s_bufferLen + len <= UART_DATA_BUF_SIZE)
		{
			std::copy(data.begin(), data.end(), s_buffer.data() + s_bufferLen);
			s_bufferLen += len;
		}
		else
		{
			LOG_ERROR("Buffer overflow");
			s_buffer.fill(0);
			s_bufferLen = 0;
			return;
		}

		if (s_buffer[s_bufferLen - 1] == '\n')
		{
			static Comm::JsonDecoder decoder;
			decoder.CheckInput(s_buffer.data(), s_bufferLen);
			s_buffer.fill(0);
			s_bufferLen = 0;
		}
	}
} // namespace SerialIo
