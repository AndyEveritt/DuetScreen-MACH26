#pragma once

#include "libusb-1.0/libusb.h"
#include <string>

namespace Comm
{
	struct UsbDeviceInterface
	{
		uint8_t inEndpoint;
		uint8_t outEndpoint;
		uint16_t packetSize;
	};

	class UsbDevice
	{
	  public:
		enum class receive_err_t
		{
			NONE = 0,
			BUFFER_TOO_SMALL = -1,
			OVERFLOW = -2,
			TIMEOUT = -3,
			BUSY = -4,
			NO_DEVICE = -5,
			OTHER_ERROR = -6
		};

		UsbDevice();
		~UsbDevice();

		bool init(const char* name, libusb_device* device);
		bool connect();
		void reset();
		ssize_t send(std::string_view data);
		receive_err_t receive(unsigned char* data, size_t len, int& received);
		bool isConnected() const { return m_handle != nullptr; }

	  private:
		int setDtr(bool state);
		bool getDeviceInterface();

		const char* m_name;
		libusb_device* m_device;
		libusb_device_handle* m_handle;
		uint8_t m_inEndpoint;
		uint8_t m_outEndpoint;
		uint16_t m_packetSize;
	};

	int usbInit();
	bool connectUsbDevice();
	UsbDevice& getCurrentUsbDevice();
	ssize_t sendUsbData(std::string_view data);
} // namespace Comm
