#include "Usb.h"
#include "Comm/JsonDecoder.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "tracy/Tracy.hpp"
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <thread>

namespace Comm
{
	static UsbDevice s_currentUsbDevice;
	static libusb_context* s_context = nullptr;

	using vendor_id_t = uint16_t;
	using product_id_t = uint16_t;

	static const std::unordered_map<vendor_id_t, std::unordered_map<product_id_t, std::string_view>> s_devices = {
		{0x1d50, {{0x60ec, "Duet 2"}, {0x60ed, "Duet 2 Maestro"}, {0x60ee, "Duet 3"}}},
		{0x16c0, {{0x27dd, "CDC-ACM Device"}}}};

	static std::recursive_mutex s_usbMutex;
	static std::mutex s_transferMutex;
	static std::condition_variable s_completionCondition;

	static constexpr int32_t s_usbTimeoutMs = 1000;

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

	bool UsbDevice::init(const char* name, libusb_device* device, receive_cb_t callback)
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		m_name = name;
		m_device = device;
		m_receiveCallback = callback;
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
			// Stop event handling thread before releasing resources
			m_eventThreadRunning = false;
			if (m_eventLoopThread.joinable())
			{
				m_eventLoopThread.join();
			}

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
				goto close_handle;
			}
		}

		r = setDtr(true);
		if (r < 0)
		{
			LOG_ERROR("Failed to set DTR: {:s}", libusb_error_name(r));
			goto close_handle;
		}

#if 0
		/* This shouldn't do anything since we are using bulk usb transfers */
		r = setBaud(115200);
		if (r < 0)
		{
			LOG_ERROR("Failed to set baud rate: {:s}", libusb_error_name(r));
			goto close_handle;
		}
#endif

		// Claim interface 0 (replace with your interface number)
		r = libusb_claim_interface(m_handle, 0);
		if (r < 0)
		{
			LOG_ERROR("Cannot claim interface: {:s}\nClosing device", libusb_error_name(r));
			goto close_handle;
		}

		m_eventThreadRunning = true;
		m_eventLoopThread = std::thread(&UsbDevice::eventLoop, this);
		receive(s_usbTimeoutMs);

		return true;

	close_handle:
		LOG_DBG("Closing device");
		libusb_close(m_handle);
		m_handle = nullptr;
		return false;
	}

	bool UsbDevice::send(std::string_view data, unsigned int timeoutMs)
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		if (!m_handle)
		{
			LOG_WARN("No USB device handle");
			return -1;
		}

		struct libusb_transfer* transfer = libusb_alloc_transfer(0);
		if (!transfer)
		{
			LOG_ERROR("Failed to allocate transfer");
			return false;
		}

		TransferData* transferData = new TransferData();
		transferData->buffer.assign(data.begin(), data.end());
		transferData->device = this;

		// Fill bulk transfer structure
		libusb_fill_bulk_transfer(transfer,
								  m_handle,
								  m_outEndpoint,
								  transferData->buffer.data(),
								  static_cast<int>(transferData->buffer.size()),
								  sendTransferCallback,
								  transferData,
								  timeoutMs);

		int r = libusb_submit_transfer(transfer);
		if (r < 0)
		{
			LOG_ERROR("Failed to submit transfer: {:s}", libusb_error_name(r));
			libusb_free_transfer(transfer);
			delete transferData;
			if (r == LIBUSB_ERROR_NO_DEVICE)
			{
				Reconnect();
			}
			return false;
		}

		return true;
	}

	// Convenience wrapper: set baud as 8N1 on CDC-ACM
	bool UsbDevice::setBaud(uint32_t baud)
	{
		return setLineCoding(baud, /*stopBits*/ 0, /*parity*/ 0, /*dataBits*/ 8);
	}

	// CDC-ACM SET_LINE_CODING to configure baud/format
	bool UsbDevice::setLineCoding(uint32_t baud, uint8_t stopBits, uint8_t parity, uint8_t dataBits)
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		if (!m_handle)
		{
			LOG_WARN("No USB device handle");
			return false;
		}

#pragma pack(push, 1)
		struct LineCoding
		{
			uint32_t dwDTERate;	 // Baud rate in bps (LE)
			uint8_t bCharFormat; // Stop bits: 0=1,1=1.5,2=2
			uint8_t bParityType; // 0=None,1=Odd,2=Even,3=Mark,4=Space
			uint8_t bDataBits;	 // Data bits: typically 5..8
		};
#pragma pack(pop)

		LineCoding lc{baud, stopBits, parity, dataBits};

		uint8_t bmRequestType = static_cast<uint8_t>(LIBUSB_ENDPOINT_OUT) |
								static_cast<uint8_t>(LIBUSB_REQUEST_TYPE_CLASS) |
								static_cast<uint8_t>(LIBUSB_RECIPIENT_INTERFACE);
		uint8_t bRequest = 0x20; // SET_LINE_CODING
		uint16_t wValue = 0;
		uint16_t wIndex = 0; // Control interface number; current implementation uses interface 0
		unsigned int timeoutMs = 1000;

		int err = libusb_control_transfer(m_handle,
										  bmRequestType,
										  bRequest,
										  wValue,
										  wIndex,
										  reinterpret_cast<unsigned char*>(&lc),
										  static_cast<uint16_t>(sizeof(lc)),
										  timeoutMs);
		if (err < 0)
		{
			LOG_ERROR("Failed to set line coding: {:s}", libusb_error_name(err));
			return false;
		}
		LOG_DBG("CDC line coding set: {} bps, {} stop, parity {}, {} bits", baud, stopBits, parity, dataBits);
		return true;
	}

	UsbDevice::receive_err_t UsbDevice::receive(unsigned int timeoutMs)
	{
		if (!m_handle)
		{
			LOG_WARN("No USB device handle");
			return receive_err_t::NO_DEVICE;
		}

		struct libusb_transfer* transfer = libusb_alloc_transfer(0);
		if (!transfer)
		{
			LOG_ERROR("Failed to allocate transfer");
			return receive_err_t::FAILED_TO_ALLOCATE_TRANSFER;
		}

		libusb_fill_bulk_transfer(transfer,
								  m_handle,
								  m_inEndpoint,
								  m_receiveBuffer,
								  s_receiveBufferSize,
								  receiveTransferCallback,
								  this,
								  timeoutMs);

		int r = libusb_submit_transfer(transfer);
		if (r < 0)
		{
			LOG_ERROR("Failed to submit transfer: {:s}", libusb_error_name(r));
			libusb_free_transfer(transfer);
			return receive_err_t::FAILED_TO_SUBMIT_TRANSFER;
		}

		return receive_err_t::NONE;
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

	void LIBUSB_CALL UsbDevice::sendTransferCallback(struct libusb_transfer* transfer)
	{
		TransferData* transferData = static_cast<TransferData*>(transfer->user_data);

		// Notify completion for any pending transfers
		std::unique_lock<std::mutex> lock(s_transferMutex);
		transferData->completed = true;

		if (transfer->status == LIBUSB_TRANSFER_COMPLETED)
		{
			// Data successfully transferred, invoke the callback
			if (transferData->callback)
			{
				transferData->callback(transferData->buffer);
			}
		}
		else
		{
			LOG_ERROR("Transfer failed: {}", libusb_error_name(transfer->status));
			// Handle error, optionally invoke callback with an empty buffer or error code
		}

		libusb_free_transfer(transfer);		// Free the transfer after processing
		delete transferData;				// Clean up user data
		s_completionCondition.notify_all(); // Notify event loop about completion
	}

	void LIBUSB_CALL UsbDevice::receiveTransferCallback(struct libusb_transfer* transfer)
	{
		auto device = static_cast<UsbDevice*>(transfer->user_data);

		// Notify completion for any pending transfers
		std::unique_lock<std::mutex> lock(s_transferMutex);

		if (transfer->status == LIBUSB_TRANSFER_COMPLETED)
		{
			// Data successfully transferred, invoke the callback
			if (device->m_receiveCallback)
			{
				device->m_receiveCallback(transfer->buffer, transfer->actual_length);
			}
		}
		else if (transfer->status == LIBUSB_TRANSFER_TIMED_OUT)
		{
			LOG_DBG("Transfer timed out");
		}
		else if (transfer->status == LIBUSB_TRANSFER_OVERFLOW)
		{
			LOG_ERROR("Transfer overflow, buffer too small");
		}
		else
		{
			LOG_ERROR("Transfer failed: {}", libusb_error_name(transfer->status));
		}

		device->receive(s_usbTimeoutMs);

		libusb_free_transfer(transfer);		// Free the transfer after processing
		s_completionCondition.notify_all(); // Notify event loop about completion
	}

	void UsbDevice::eventLoop()
	{
		tracy::SetThreadName("USB Event Loop");
		timeval tv = {0, 50'000}; // 50 ms
		while (m_eventThreadRunning)
		{
			int r = libusb_handle_events_timeout(s_context, &tv); // blocking call
			if (r == LIBUSB_ERROR_INTERRUPTED)
			{
				continue;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Prevent busy-waiting
		}
	}

	UsbDevice& getCurrentUsbDevice()
	{
		return s_currentUsbDevice;
	}

	static bool findDuetUsbDevice(libusb_device** device_list,
								  ssize_t device_count,
								  const char** found_device_name,
								  libusb_device** found_device)
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		for (ssize_t i = 0; i < device_count; ++i)
		{
			libusb_device* device = device_list[i];
			libusb_device_descriptor desc;
			if (libusb_get_device_descriptor(device, &desc) != 0)
			{
				continue;
			}

			auto vendorDevicesIt = s_devices.find(desc.idVendor);
			if (vendorDevicesIt == s_devices.end())
			{
				continue;
			}

			auto& productDevices = vendorDevicesIt->second;
			auto productDeviceIt = productDevices.find(desc.idProduct);
			if (productDeviceIt == productDevices.end())
			{
				continue;
			}

			std::string_view device_name = productDeviceIt->second;

			LOG_INFO("{:s} target device (Vendor ID: {:#x}, Product ID: {:#x}) found.",
					 device_name,
					 desc.idVendor,
					 desc.idProduct);
			*found_device_name = device_name.data();
			*found_device = device;
			return true;
		}

		*found_device_name = nullptr;
		*found_device = nullptr;
		return false;
	}

	int usbInit()
	{
		std::lock_guard<std::recursive_mutex> lock(s_usbMutex);
		return libusb_init(&s_context);
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
		ssize_t device_count = libusb_get_device_list(s_context, &device_list);

		if (device_count < 0)
		{
			LOG_ERROR("Failed to get device list: {:s}", libusb_error_name(static_cast<int>(device_count)));
			return false;
		}

		bool ret = true;
		const char* device_name = nullptr;
		libusb_device* device = nullptr;
		if (!findDuetUsbDevice(device_list, device_count, &device_name, &device))
		{
			LOG_ERROR("Target device not found");
			ret = false;
			goto finish;
		}

		s_currentUsbDevice.init(device_name,
								device,
								[](unsigned char* buf, size_t len)
								{
									static Comm::JsonDecoder s_decoder;
									s_decoder.CheckInput(buf, len);
								});

		if (!s_currentUsbDevice.connect())
		{
			LOG_ERROR("Failed to connect to target device");
			ret = false;
			goto finish;
		}

	finish:
		libusb_free_device_list(device_list, 1);
		return ret;
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
