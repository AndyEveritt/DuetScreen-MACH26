#include "UartController.h"
#include "Debug.h"
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
	info("Simulating UART open on device: %s", device.c_str());
	m_fd = 1; // Simulate success
	m_running = true;
	m_readThread = std::thread(&UartController::readLoop, this);
	return true;
#else
	if (isOpen())
	{
		close();
	}

	m_fd = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (m_fd < 0)
	{
		error("Failed to open UART device %s", device.c_str());
		return false;
	}

	if (!configurePort())
	{
		::close(m_fd);
		m_fd = -1;
		return false;
	}

	// Start read thread
	m_running = true;
	m_readThread = std::thread(&UartController::readLoop, this);

	return true;
#endif
}

void UartController::close()
{
	if (isOpen())
	{
		m_running = false;
		if (m_readThread.joinable())
		{
			m_readThread.join();
		}
#if !SIMULATION
		::close(m_fd);
#endif
		m_fd = -1;
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
		error("Error from tcgetattr");
		return false;
	}

	// Set baud rate
	if (cfsetospeed(&tty, m_currentBaudRate) != 0)
	{
		error("Error setting output baud rate");
		return false;
	}
	if (cfsetispeed(&tty, m_currentBaudRate) != 0)
	{
		error("Error setting input baud rate");
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
		error("Error from tcsetattr");
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

bool UartController::send(const uint8_t* data, size_t length)
{
#if SIMULATION
	info("Simulated UART send: %.*s", (int)length, data);
	return true;
#else
	if (!isOpen())
	{
		return false;
	}

	size_t written = 0;
	while (written < length)
	{
		ssize_t ret = write(m_fd, data + written, length - written);
		if (ret < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				continue;
			}
			error("UART write error: %s", strerror(errno));
			return false;
		}
		written += ret;
	}

	return true;
#endif
}

void UartController::readLoop()
{
#if SIMULATION
	// Simulate periodic data reception
	std::vector<uint8_t> buffer(m_bufferSize);
	while (m_running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (m_receiveCallback)
		{
			const char* testData = "simulated data\n";
			size_t len = strlen(testData);
			memcpy(buffer.data(), testData, len);
			m_receiveCallback(buffer.data(), len);
		}
	}
#else
	std::vector<uint8_t> buffer(m_bufferSize);

	while (m_running)
	{
		ssize_t bytesRead = read(m_fd, buffer.data(), buffer.size());

		if (bytesRead > 0)
		{
			// Make a local copy of callback with lock protection
			DataCallback callback;
			{
				std::lock_guard<std::mutex> lock(m_callbackMutex);
				callback = m_receiveCallback;
			}

			// Only call if we have a valid callback
			if (callback)
			{
				callback(buffer.data(), bytesRead);
			}
		}
		else if (bytesRead < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
		{
			error("UART read error: %s", strerror(errno));
			break;
		}
	}
#endif
}
