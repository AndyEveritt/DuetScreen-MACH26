#include "UartController.h"
#include "Debug.h"
#include "tracy/Tracy.hpp"
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

UartController::UartController()
	: m_fd(-1)
	, m_running(false)
	, m_bufferSize(DEFAULT_BUFFER_SIZE)
	, m_currentBaudRate(B115200)
{
}

UartController::~UartController()
{
	close();
}

bool UartController::open(const std::string& device)
{
#if SIMULATION
	LOG_INFO("Simulating UART open on device: {:s}", device.c_str());
	m_fd = 1; // Simulate success
#else
	if (isOpen())
	{
		close();
	}

	LOG_INFO("Opening UART device {:s}", device.c_str());

	m_fd = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (m_fd < 0)
	{
		LOG_ERROR("Failed to open UART device {:s}", device.c_str());
		return false;
	}

	if (!configurePort())
	{
		::close(m_fd);
		m_fd = -1;
		return false;
	}
#endif

	// Start read thread
	m_running = true;
	m_readThread = std::thread(&UartController::readLoop, this);

	return true;
}

void UartController::close()
{
	if (isOpen())
	{
		LOG_DBG("Closing UART device"); // Caused a segfault in release mode for unit tests when level was info?
		// Signal the read thread to stop first
		m_running = false;

		// Close the file descriptor to interrupt any blocking read
#if !SIMULATION
		::close(m_fd);
#endif
		m_fd = -1;

		if (m_readThread.joinable())
		{
			m_readThread.join();
		}
	}
}

bool UartController::configurePort()
{
#if SIMULATION
	return true;
#else
	struct termios tty;

	if (tcgetattr(m_fd, &tty) != 0)
	{
		LOG_ERROR("Error from tcgetattr");
		return false;
	}

	// Set baud rate
	if (cfsetospeed(&tty, m_currentBaudRate) != 0)
	{
		LOG_ERROR("Error setting output baud rate");
		return false;
	}
	if (cfsetispeed(&tty, m_currentBaudRate) != 0)
	{
		LOG_ERROR("Error setting input baud rate");
		return false;
	}

	tty.c_cflag |= (CLOCAL | CREAD); // Ignore modem controls
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;		 // 8-bit characters
	tty.c_cflag &= ~PARENB;	 // No parity bit
	tty.c_cflag &= ~CSTOPB;	 // 1 stop bit
	tty.c_cflag &= ~CRTSCTS; // No hardware flow control

	// Setup for non-canonical mode
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	// Fetch bytes as they become available
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 1;

	if (tcsetattr(m_fd, TCSANOW, &tty) != 0)
	{
		LOG_ERROR("Error from tcsetattr");
		return false;
	}

	return true;
#endif
}

bool UartController::setBaudRate(speed_t baudRate)
{
	m_currentBaudRate = baudRate;
#if SIMULATION
	return true;
#else
	return !isOpen() || configurePort();
#endif
}

bool UartController::setParameters(int dataBits, int stopBits, char parity)
{
	LOG_DBG("Setting UART parameters: dataBits={:d}, stopBits={:d}, parity={:c}", dataBits, stopBits, parity);
#if SIMULATION
	return true;
#else
	struct termios tty;
	if (tcgetattr(m_fd, &tty) != 0)
	{
		return false;
	}

	tty.c_cflag &= ~CSIZE;
	switch (dataBits)
	{
	case 5:
		tty.c_cflag |= CS5;
		break;
	case 6:
		tty.c_cflag |= CS6;
		break;
	case 7:
		tty.c_cflag |= CS7;
		break;
	default:
		tty.c_cflag |= CS8;
		break;
	}

	tty.c_cflag &= ~CSTOPB;
	if (stopBits == 2)
	{
		tty.c_cflag |= CSTOPB;
	}

	tty.c_cflag &= ~PARENB;
	if (parity != 'N')
	{
		tty.c_cflag |= PARENB;
		if (parity == 'O')
		{
			tty.c_cflag |= PARODD;
		}
	}

	return tcsetattr(m_fd, TCSANOW, &tty) == 0;
#endif
}

void UartController::setBufferSize(size_t size)
{
	m_bufferSize = size;
}

ssize_t UartController::send(std::string_view data)
{
	LOG_VERBOSE("Sending data: {}", data);
	return _send((const uint8_t*)data.data(), data.length());
}

ssize_t UartController::_send(const uint8_t* data, size_t length)
{
#if SIMULATION
	LOG_INFO("Simulated UART send: {:.{}s}", reinterpret_cast<const char*>(data), (int)length);
	return length;
#else
	if (!isOpen())
	{
		return 0;
	}

	std::lock_guard<std::mutex> lock(m_writeMutex);
	ssize_t written = 0;
	while (written < (ssize_t)length)
	{
		ssize_t ret = write(m_fd, data + written, length - written);
		if (ret < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				continue;
			}
			LOG_ERROR("UART write error: {:s}, written {}/{}", strerror(errno), written, length);
			return written;
		}
		written += ret;
	}

	return written;
#endif
}

void UartController::readLoop()
{
	tracy::SetThreadName("UartController::readLoop");
	std::vector<uint8_t> buffer(m_bufferSize);
#if SIMULATION
	while (m_running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (m_receiveCallback)
		{
			// TODO simulate incoming data
			const char* testData = "{\"key\": \"value\"}\n";
			size_t len = strlen(testData);
			memcpy(buffer.data(), testData, len);
			m_receiveCallback(std::string_view(reinterpret_cast<char*>(buffer.data()), len));
		}
	}
#else
	fd_set readfds;
	struct timeval tv;

	while (m_running)
	{
		FD_ZERO(&readfds);
		FD_SET(m_fd, &readfds);

		tv.tv_sec = 0;
		tv.tv_usec = 100000; // 100ms timeout

		int ret = select(m_fd + 1, &readfds, nullptr, nullptr, &tv);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue;
			LOG_ERROR("UART select error: {:s}", strerror(errno));
			break;
		}

		if (ret == 0 || !m_running)
			continue;

		ssize_t bytesRead = read(m_fd, buffer.data(), buffer.size());

		if (bytesRead > 0)
		{
			DataCallback callback;
			{
				std::lock_guard<std::mutex> lock(m_callbackMutex);
				callback = m_receiveCallback;
			}

			if (callback)
			{
				callback(std::string_view(reinterpret_cast<char*>(buffer.data()), bytesRead));
			}
		}
		else if (bytesRead < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
		{
			LOG_ERROR("UART read error: {:s}", strerror(errno));
			break;
		}
	}
#endif
}
