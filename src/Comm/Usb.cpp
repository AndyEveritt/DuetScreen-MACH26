#include "Usb.h"
#include "Debug.h"
#include <cstring>
#include <pthread.h>

namespace Comm
{
	static UsbDevice s_currentUsbDevice;
	static libusb_context* s_context = nullptr;
	static pthread_mutex_t s_usbMutex = PTHREAD_MUTEX_INITIALIZER;

	static const uint16_t vendorId = 0x1d50;
	struct UsbDeviceId
	{
		const char* name;
		uint16_t productId;
	};

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
		m_name = name;
		m_device = device;
		if (!getDeviceInterface())
		{
			error("Failed to get device interface");
			return false;
		}
		return true;
	}

	void UsbDevice::reset()
	{
		pthread_mutex_lock(&s_usbMutex);
		dbg("Resetting USB device %s", m_name);
		if (m_handle)
		{
			dbg("Releasing interface");
			libusb_release_interface(m_handle, 0);
			libusb_close(m_handle);
			m_handle = nullptr;
		}
		if (m_device)
		{
			dbg("Unref device");
			libusb_unref_device(m_device);
			m_device = nullptr;
		}
		m_name = "";
		m_inEndpoint = 0;
		m_outEndpoint = 0;
		m_packetSize = 0;
		pthread_mutex_unlock(&s_usbMutex);
	}

	bool UsbDevice::connect()
	{
		int r;

		if (!m_device)
		{
			error("No USB device");
			return false;
		}

		libusb_open(m_device, &m_handle);

		if (!m_handle)
		{
			error("Cannot open device");
			return false;
		}

		// Detach the kernel driver if necessary
		if (libusb_kernel_driver_active(m_handle, 0) == 1)
		{
			r = libusb_detach_kernel_driver(m_handle, 0);
			if (r < 0)
			{
				fprintf(stderr, "Cannot detach kernel driver: %s\n", libusb_error_name(r));
				libusb_close(m_handle);
				return false;
			}
		}

		setDtr(true);

		// Claim interface 0 (replace with your interface number)
		r = libusb_claim_interface(m_handle, 0);
		if (r < 0)
		{
			fprintf(stderr, "Cannot claim interface: %s\n", libusb_error_name(r));
			libusb_close(m_handle);
			return false;
		}

		return true;
	}

	int UsbDevice::send(const char* data)
	{
		if (!m_handle)
		{
			warn("No USB device handle");
			return -1;
		}
		int full_length = 0;
		int actual_length = 0;
		size_t len = strlen(data);

		while (len > 0)
		{
			int lenToSend = len > m_packetSize ? m_packetSize : len;
			int r = libusb_bulk_transfer(m_handle, m_outEndpoint, (unsigned char*)data, len, &actual_length, 0);
			if (r != 0)
			{
				error("Error sending data: %s", libusb_error_name(r));
				reset();
				return -1;
			}
			len -= actual_length;
			data += actual_length;
			full_length += actual_length;
		}
		return full_length;
	}

	int UsbDevice::receive(unsigned char* data, size_t len)
	{
		if (!m_handle)
		{
			warn("No USB device handle");
			return -1;
		}

		if (len < m_packetSize)
		{
			warn("Buffer too small");
			return -1;
		}

		int actual_length = 0;
		int r = libusb_bulk_transfer(m_handle, m_inEndpoint, data, m_packetSize, &actual_length, 1000);
		switch (r)
		{
		case LIBUSB_SUCCESS:
			break;
		case LIBUSB_ERROR_TIMEOUT:
			warn("No more data received (timeout)");
			break;
		case LIBUSB_ERROR_BUSY:
			warn("Busy receiving data");
			break;
		case LIBUSB_ERROR_NO_DEVICE:
			warn("Device disconnected");
			reset();
			break;
		default:
			error("Error receiving data: %s", libusb_error_name(r));
			reset();
			break;
		}

		return actual_length;
	}

	int UsbDevice::setDtr(bool state)
	{
		if (!m_handle)
		{
			warn("No USB device handle");
			return -1;
		}
		uint8_t request_type = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;
		uint8_t request = 0x22;				  // SET_CONTROL_LINE_STATE (commonly used for DTR/RTS)
		uint16_t value = state ? 0x01 : 0x00; // DTR set high (bit 0)
		uint16_t index = 0;					  // Interface number (adjust if necessary)
		int err = libusb_control_transfer(m_handle, request_type, request, value, index, nullptr, 0, 1000);
		if (err < 0)
		{
			error("Failed to set DTR: %s", libusb_error_name(err));
		}
		else
		{
			dbg("DTR set successfully.");
		}
		return err;
	}

	bool UsbDevice::getDeviceInterface()
	{
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
							dbg("Found IN endpoint: %x", m_inEndpoint);
							foundIn = true;
						}
						else
						{
							m_outEndpoint = ep_desc.bEndpointAddress;
							dbg("Found OUT endpoint: %x", m_outEndpoint);
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
						if (desc.idProduct == deviceId.productId)
						{
							info("%s target device (Product ID: %x) found.", deviceId.name, deviceId.productId);
							return s_currentUsbDevice.init(deviceId.name, device);
						}
					}
				}
			}
		}
		return false;
	}

	bool connectUsbDevice()
	{
		int r;

		s_currentUsbDevice.reset();

		pthread_mutex_lock(&s_usbMutex);
		if (s_context != nullptr)
		{
			libusb_exit(nullptr);
			s_context = nullptr;
		}

		libusb_init(nullptr);

		verbose("Getting usb device list");
		libusb_device** device_list;
		ssize_t device_count = libusb_get_device_list(nullptr, &device_list);

		if (device_count < 0)
		{
			error("Failed to get device list: %s", libusb_error_name(device_count));
			pthread_mutex_unlock(&s_usbMutex);
			return false;
		}

		if (!findDuetUsbDevice(device_list, device_count))
		{
			error("Target device not found");
			libusb_free_device_list(device_list, 1);
			pthread_mutex_unlock(&s_usbMutex);
			return false;
		}

		if (!s_currentUsbDevice.connect())
		{
			error("Failed to connect to target device");
			libusb_free_device_list(device_list, 1);
			pthread_mutex_unlock(&s_usbMutex);
			return false;
		}
		libusb_free_device_list(device_list, 1);
		pthread_mutex_unlock(&s_usbMutex);

		return true;
	}

} // namespace Comm
