#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <libusb-1.0/libusb.h>
#include <string>
#include <thread>

namespace Comm
{
	class UsbDevice;

	// Define a structure to pass user data to the callback
	using transfer_cb_t = std::function<void(const std::vector<unsigned char>&)>;
	using receive_cb_t = std::function<void(unsigned char* buf, size_t len)>;
	struct TransferData
	{
		std::vector<unsigned char> buffer;
		transfer_cb_t callback = nullptr;
		bool completed = false; // Flag for transfer completion
		bool receiving = false;
		UsbDevice* device = nullptr; // Pointer to the UsbDevice instance
	};

	class UsbDevice
	{
	  public:
		enum class receive_err_t
		{
			NONE = 0,
			FAILED_TO_ALLOCATE_TRANSFER = -1,
			FAILED_TO_SUBMIT_TRANSFER = -2,
			BUFFER_TOO_SMALL = -3,
			BUFFER_OVERFLOW = -4,
			TIMEOUT = -5,
			BUSY = -6,
			NO_DEVICE = -7,
			OTHER_ERROR = -8
		};

		UsbDevice();
		~UsbDevice();

		bool init(const char* name, libusb_device* device, receive_cb_t callback = nullptr);
		bool connect();
		void reset();
		bool send(std::string_view data, unsigned int timeoutMs = 0);
		bool isConnected() const { return m_handle != nullptr; }

	  private:
		int setDtr(bool state);

		// CDC-ACM line coding helpers
		// Convenience: set baud as 8N1
		bool setBaud(uint32_t baud);
		// Full control: baud, stop bits (0=1,1=1.5,2=2), parity (0=None..4=Space), data bits
		bool setLineCoding(uint32_t baud, uint8_t stopBits, uint8_t parity, uint8_t dataBits);

		bool getDeviceInterface();

		static void LIBUSB_CALL sendTransferCallback(struct libusb_transfer* transfer);
		static void LIBUSB_CALL receiveTransferCallback(struct libusb_transfer* transfer);
		void eventLoop();
		receive_err_t receive(unsigned int timeoutMs = 0);

		const char* m_name;
		libusb_device* m_device;
		libusb_device_handle* m_handle;

		uint8_t m_inEndpoint;
		uint8_t m_outEndpoint;
		uint16_t m_packetSize;
		uint8_t m_dataInterfaceNumber;
		uint8_t m_controlInterfaceNumber;
		std::size_t m_claimedInterfaceCount;
		uint8_t m_claimedInterfaces[2];

		std::atomic<bool> m_eventThreadRunning;
		std::thread m_eventLoopThread;

		TransferData m_receiveTransferData;
		receive_cb_t m_receiveCallback;
		static constexpr size_t s_receiveBufferSize = 4096;
		unsigned char m_receiveBuffer[s_receiveBufferSize];
	};

	int usbInit();
	bool connectUsbDevice();
	UsbDevice& getCurrentUsbDevice();
	ssize_t sendUsbData(std::string_view data);

	enum class UsbMode
	{
		Host,
		Device,
		InternalWiFi,
		Unknown
	};
	void setUsbMode(const UsbMode mode);
	UsbMode getUsbMode();
} // namespace Comm
