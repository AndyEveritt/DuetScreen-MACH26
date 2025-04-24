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

	static void processData(const uint8_t* data, size_t len);

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
		s_uart->setReceiveCallback(processData);

		return s_uart->open(device);
	}

	void Shutdown()
	{
		if (s_uart)
		{
			s_uart->close();
			s_uart.reset();
		}
	}

	bool Send(const std::string& data)
	{
		return s_uart && s_uart->send(data);
	}

	size_t Sendf(const char* fmt, ...)
	{
		va_list vargs;
		va_start(vargs, fmt);

		std::string buf = utils::vformat(fmt, vargs);
		LOG_INFO("Sending {:s}", buf.c_str());

		if (s_uart)
		{
			s_uart->send(buf);
		}

		va_end(vargs);
		return buf.length();
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

	static void processData(const uint8_t* data, size_t len)
	{
		LOG_VERBOSE("Received {1:.{0}s}", (int)len, data);

		if (Comm::DUET.GetCommunicationType() != Comm::CommunicationType::uart)
		{
			s_bufferLen = 0;
			return;
		}

		if (s_bufferLen + len <= UART_DATA_BUF_SIZE)
		{
			std::copy(data, data + len, s_buffer.data() + s_bufferLen);
			s_bufferLen += len;
		}
		else
		{
			LOG_ERROR("Buffer overflow");
			s_buffer.fill(0);
			s_bufferLen = 0;
			Comm::Reconnect();
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
