#pragma once

#include "libusb-1.0/libusb.h"

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
		UsbDevice();
		~UsbDevice();

		bool init(const char* name, libusb_device* device);
		bool connect();
		void reset();
		int send(const char* data);
		int receive(unsigned char* data, size_t len);
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

} // namespace Comm
