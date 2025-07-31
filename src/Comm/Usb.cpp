#include "Usb.h"
#include "Comm/JsonDecoder.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include <atomic>
#include <cstring>
#include <mutex>
#include <thread>

namespace Comm
{
	static UsbDevice s_currentUsbDevice;
	static libusb_context* s_context = nullptr;

	static const uint16_t vendorId = 0x1d50;
	struct UsbDeviceId
	{
		const char* name;
		uint16_t productId;
	};

	std::recursive_mutex s_usbMutex;

	static UsbDeviceId s_deviceIds[] = {
		{"Duet 2", 0x60ec},
		{"Duet 2 Maestro", 0x60ed},
		{"Duet 3", 0x60ee},
	};

	UsbDevice::UsbDevice()
		: m_name("")
		, m_device(nullptr)
		, m_handle(nullptr)
		, m_inEndpoint(0)
		, m_outEndpoint(0)
		, m_packetSize(0)
	{
	}

	UsbDevice::~UsbDevice()
	{
		reset();
	}

	bool UsbDevice::init(const char* name, libusb_device* device)
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		m_name = name;
		m_device = device;
		if (!getDeviceInterface())
		{
			LOG_ERROR("Failed to get device interface");
			return false;
		}
		return true;
	}

	void UsbDevice::reset()
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		LOG_DBG("Resetting USB device {:s}", m_name);
		if (m_handle)
		{
			LOG_DBG("Releasing interface");
			libusb_release_interface(m_handle, 0);
			libusb_close(m_handle);
			m_handle = nullptr;
		}
		if (m_device)
		{
			LOG_DBG("Unref device");
			// libusb_unref_device(m_device);
			m_device = nullptr;
		}
		m_name = "";
		m_inEndpoint = 0;
		m_outEndpoint = 0;
		m_packetSize = 0;
	}

	bool UsbDevice::connect()
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		int r;

		if (!m_device)
		{
			LOG_ERROR("No USB device");
			return false;
		}

		libusb_open(m_device, &m_handle);

		if (!m_handle)
		{
			LOG_ERROR("Cannot open device");
			return false;
		}

		// Detach the kernel driver if necessary
		if (libusb_kernel_driver_active(m_handle, 0) == 1)
		{
			r = libusb_detach_kernel_driver(m_handle, 0);
			if (r < 0)
			{
				LOG_ERROR("Cannot detach kernel driver: {:s}", libusb_error_name(r));
				libusb_close(m_handle);
				return false;
			}
		}

		r = setDtr(true);
		if (r < 0)
		{
			LOG_ERROR("Closing device");
			libusb_close(m_handle);
			m_handle = nullptr;
			return false;
		}

		// Claim interface 0 (replace with your interface number)
		r = libusb_claim_interface(m_handle, 0);
		if (r < 0)
		{
			LOG_ERROR("Cannot claim interface: {:s}\nClosing device", libusb_error_name(r));
			libusb_close(m_handle);
			m_handle = nullptr;
			return false;
		}

		return true;
	}

	ssize_t UsbDevice::send(std::string_view data)
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		if (!m_handle)
		{
			LOG_WARN("No USB device handle");
			return -1;
		}
		ssize_t full_length = 0;
		int actual_length = 0;
		size_t len = data.length();
		const char* ptr = data.data();

		LOG_VERBOSE("Sending data: {}, length: {:d}", data, len);

		while (len > 0)
		{
			int lenToSend = len > m_packetSize ? m_packetSize : len;
			int r = libusb_bulk_transfer(m_handle, m_outEndpoint, (unsigned char*)ptr, lenToSend, &actual_length, 0);
			if (r != 0)
			{
				LOG_ERROR("Error sending data: {:s}, sent {}/{}", libusb_error_name(r), full_length, len);
				reset();
				return -1;
			}
			len -= actual_length;
			ptr += actual_length;
			full_length += actual_length;
		}
		return full_length;
	}

	UsbDevice::receive_err_t UsbDevice::receive(unsigned char* data, size_t len, int& received)
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		if (!m_handle)
		{
			LOG_WARN("No USB device handle");
			return receive_err_t::NO_DEVICE;
		}

		if (len < m_packetSize)
		{
			LOG_WARN("Buffer too small, must be at least {:d} bytes", m_packetSize);
			return receive_err_t::BUFFER_TOO_SMALL;
		}

		received = 0;
		int r = libusb_bulk_transfer(m_handle, m_inEndpoint, data, len, &received, 1000);
		LOG_VERBOSE(
			"Received {:d} bytes: {:s}", received, std::string_view(reinterpret_cast<const char*>(data), received));
		switch (r)
		{
		case LIBUSB_SUCCESS:
			return receive_err_t::NONE;
		case LIBUSB_ERROR_TIMEOUT:
			LOG_DBG("No more data received (timeout)");
			return receive_err_t::TIMEOUT;
		case LIBUSB_ERROR_BUSY:
			LOG_WARN("Busy receiving data");
			return receive_err_t::BUSY;
		case LIBUSB_ERROR_NO_DEVICE:
			LOG_WARN("Device disconnected");
			reset();
			return receive_err_t::NO_DEVICE;
		case LIBUSB_ERROR_IO:
		case LIBUSB_ERROR_PIPE:
		case LIBUSB_ERROR_OVERFLOW:
		default:
			LOG_ERROR("Error receiving data: {:s}", libusb_error_name(r));
			reset();
			return receive_err_t::OTHER_ERROR;
		}
	}

	int UsbDevice::setDtr(bool state)
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		if (!m_handle)
		{
			LOG_WARN("No USB device handle");
			return -1;
		}
		uint8_t request_type =
			static_cast<uint8_t>(LIBUSB_REQUEST_TYPE_CLASS) | static_cast<uint8_t>(LIBUSB_RECIPIENT_INTERFACE);
		uint8_t request = 0x22;				  // SET_CONTROL_LINE_STATE (commonly used for DTR/RTS)
		uint16_t value = state ? 0x01 : 0x00; // DTR set high (bit 0)
		uint16_t index = 0;					  // Interface number (adjust if necessary)
		int err = libusb_control_transfer(m_handle, request_type, request, value, index, nullptr, 0, 1000);
		if (err < 0)
		{
			LOG_ERROR("Failed to set DTR: {:s}", libusb_error_name(err));
		}
		else
		{
			LOG_DBG("DTR set successfully.");
		}
		return err;
	}

	bool UsbDevice::getDeviceInterface()
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		libusb_config_descriptor* config_desc;
		libusb_get_active_config_descriptor(m_device, &config_desc);

		bool foundIn = false;
		bool foundOut = false;

		for (int i = 0; i < config_desc->bNumInterfaces; i++)
		{
			const libusb_interface& interface = config_desc->interface[i];
			for (int j = 0; j < interface.num_altsetting; j++)
			{
				const libusb_interface_descriptor& altsetting = interface.altsetting[j];
				for (int k = 0; k < altsetting.bNumEndpoints; k++)
				{
					const libusb_endpoint_descriptor& ep_desc = altsetting.endpoint[k];
					if ((ep_desc.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK)
					{
						if (ep_desc.bEndpointAddress & LIBUSB_ENDPOINT_IN)
						{
							m_inEndpoint = ep_desc.bEndpointAddress;
							m_packetSize = ep_desc.wMaxPacketSize;
							LOG_DBG("Found IN endpoint: {:#x}", m_inEndpoint);
							foundIn = true;
						}
						else
						{
							m_outEndpoint = ep_desc.bEndpointAddress;
							LOG_DBG("Found OUT endpoint: {:#x}", m_outEndpoint);
							foundOut = true;
						}
					}
				}
			}
		}
		return foundIn && foundOut;
	}

	UsbDevice& getCurrentUsbDevice()
	{
		return s_currentUsbDevice;
	}

	static bool findDuetUsbDevice(libusb_device** device_list, ssize_t device_count)
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		for (ssize_t i = 0; i < device_count; ++i)
		{
			libusb_device* device = device_list[i];
			libusb_device_descriptor desc;
			if (libusb_get_device_descriptor(device, &desc) == 0)
			{
				if (desc.idVendor == vendorId)
				{
					for (UsbDeviceId deviceId : s_deviceIds)
					{
						LOG_DBG("Found device {:s} (Vendor ID: {:#x}, Product ID: {:#x})",
								deviceId.name,
								vendorId,
								desc.idProduct);
						if (desc.idProduct == deviceId.productId)
						{
							LOG_INFO(
								"{:s} target device (Product ID: {:#x}) found.", deviceId.name, deviceId.productId);
							return s_currentUsbDevice.init(deviceId.name, device);
						}
					}
				}
			}
		}
		return false;
	}

	int usbInit()
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		return libusb_init(nullptr);
	}

	bool connectUsbDevice()
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		// Reset any existing connection first
		s_currentUsbDevice.reset();

		// Small delay to allow USB reset to complete
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		LOG_VERBOSE("Getting usb device list");
		libusb_device** device_list;
		ssize_t device_count = libusb_get_device_list(nullptr, &device_list);

		if (device_count < 0)
		{
			LOG_ERROR("Failed to get device list: {:s}", libusb_error_name(device_count));
			return false;
		}

		if (!findDuetUsbDevice(device_list, device_count))
		{
			LOG_ERROR("Target device not found");
			libusb_free_device_list(device_list, 1);
			return false;
		}

		if (!s_currentUsbDevice.connect())
		{
			LOG_ERROR("Failed to connect to target device");
			libusb_free_device_list(device_list, 1);
			return false;
		}
		libusb_free_device_list(device_list, 1);

		return true;
	}

	ssize_t sendUsbData(std::string_view data)
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		if (!s_currentUsbDevice.isConnected())
		{
			LOG_WARN("USB device not connected");
			return -1;
		}
		return s_currentUsbDevice.send(data);
	}
} // namespace Comm
