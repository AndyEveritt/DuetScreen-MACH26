
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include "Comm/Communication.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "glob.h"
#include "hv/requests.h"
#include "lv_i18n/lv_i18n.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/lvgl.h"
#include "utils/StorageHelper.h"
#include <libusb-1.0/libusb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "UI/Screens/Home/view.h"

#if LV_USE_OS == LV_OS_PTHREAD
#include <thread>
#elif LV_USE_OS == LV_OS_FREERTOS
#include "freertos_main.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t* hal_init(int32_t w, int32_t h);
static void http_test();
static int usb_test();

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_OS == LV_OS_PTHREAD
static pthread_t s_responseThread;
static pthread_t s_requestThread;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *      VARIABLES
 **********************/

int main(int argc, char** argv)
{
	(void)argc; /*Unused*/
	(void)argv; /*Unused*/

	// Initialise
	StorageHelper::load();
	SetDebugLevel(StorageHelper::getData(ID_DEBUG_LEVEL, DebugLevel::Info));

	Model::get(); // Initialize the model instance, this creates the subscribers

	usb_test();

	Comm::init();
	Comm::DUET.Init();

	// Comm::DUET.RequestModel("move", "vn");

	// http_test();

	/*Initialize LVGL*/
	lv_init();
	lv_i18n_init(lv_i18n_language_pack);
	lv_i18n_set_locale(StorageHelper::getData<std::string>(ID_SYS_LANG_CODE_KEY, DEFAULT_LANGUAGE_CODE).c_str());

	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	hal_init(1024, 600);

#if LV_USE_OS == LV_OS_PTHREAD

	// lv_demo_widgets();
	// lv_demo_benchmark();

	UI::HomeView home;
	home.show();

	// Create a thread to handle requesting data from Duet
	pthread_create(
		&s_requestThread,
		NULL,
		[](void*) -> void*
		{
			while (1)
			{
				// Request next section of the OM
				Comm::sendNext();
				usleep(Comm::DUET.GetScaledPollInterval() * 1000);
				// std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
			return nullptr;
		},
		NULL);

	// Create a thread to handle USB responses from Duet
	pthread_create(
		&s_responseThread,
		NULL,
		[](void*) -> void*
		{
			while (1)
			{
				usleep(1000 * 1000);
			}
			return nullptr;
		},
		NULL);

	while (1)
	{
		lv_timer_handler();
		usleep(5 * 1000); // Sleep for 1 second
	}

#elif LV_USE_OS == LV_OS_FREERTOS

	/* Run FreeRTOS and create lvgl task */
	freertos_main();

#endif

	return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t* hal_init(int32_t w, int32_t h)
{

	lv_group_set_default(lv_group_create());

	lv_display_t* disp = lv_sdl_window_create(w, h);

	lv_indev_t* mouse = lv_sdl_mouse_create();
	lv_indev_set_group(mouse, lv_group_get_default());
	lv_indev_set_display(mouse, disp);
	lv_display_set_default(disp);

	LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
	lv_obj_t* cursor_obj;
	cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
	lv_image_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
	lv_indev_set_cursor(mouse, cursor_obj);			  /*Connect the image  object to the driver*/

	lv_indev_t* mousewheel = lv_sdl_mousewheel_create();
	lv_indev_set_display(mousewheel, disp);
	lv_indev_set_group(mousewheel, lv_group_get_default());

	lv_indev_t* kb = lv_sdl_keyboard_create();
	lv_indev_set_display(kb, disp);
	lv_indev_set_group(kb, lv_group_get_default());

	return disp;
}

static void http_test()
{
	auto resp = requests::get("http://192.168.1.227/rr_model?key=move");
	// auto resp = requests::get("http://duet2.local/rr_model?key=move");
	if (resp == NULL)
	{
		error("%s", _("request_failed"));
	}
	else
	{
		printf("%s\n", resp->body.c_str());
	}
}

static libusb_device_handle* s_activeUsbDevice;

static void set_dtr_high(libusb_device_handle* handle)
{
	// Example control transfer to set DTR high.
	// Vendor-specific request. Adjust bRequest, wValue, and wIndex as per your device's documentation.
	uint8_t request_type = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;
	uint8_t request = 0x22; // SET_CONTROL_LINE_STATE (commonly used for DTR/RTS)
	uint16_t value = 0x01;	// DTR set high (bit 0)
	uint16_t index = 0;		// Interface number (adjust if necessary)
	unsigned char* data = nullptr;
	uint16_t length = 0;
	int timeout = 1000; // Timeout in milliseconds

	int err = libusb_control_transfer(handle, request_type, request, value, index, data, length, timeout);
	if (err < 0)
	{
		error("Failed to set DTR high: %s", libusb_error_name(err));
	}
	else
	{
		dbg("DTR set high successfully.");
	}
}

static int usb_test()
{
	libusb_context* ctx = nullptr;
	int r;
	unsigned char data[512];		  // Data to send
	unsigned char received_data[512]; // Buffer to receive data
	int actual_length;

	uint16_t vendor_id = 0x1d50;
	uint16_t product_ids[] = {0x60ee, 0x60ec};

	// Initialize libusb
	verbose("Initializing libusb");
	r = libusb_init(&ctx);
	if (r < 0)
	{
		fprintf(stderr, "Failed to initialize libusb\n");
		return 1;
	}

	verbose("Getting usb device list");
	libusb_device** device_list;
	ssize_t device_count = libusb_get_device_list(ctx, &device_list);

	if (device_count < 0)
	{
		std::cerr << "Failed to get device list: " << libusb_error_name(device_count) << std::endl;
		return 1;
	}

	s_activeUsbDevice = nullptr;

	libusb_device* device = nullptr;
	uint8_t in_endpoint = 0;
	uint8_t out_endpoint = 0;
	uint16_t packet_size = 0;

	for (ssize_t i = 0; i < device_count; ++i)
	{
		libusb_device* tmp_device = device_list[i];
		libusb_device_descriptor desc;
		if (libusb_get_device_descriptor(tmp_device, &desc) == 0)
		{
			if (desc.idVendor == vendor_id)
			{
				for (uint16_t pid : product_ids)
				{
					if (desc.idProduct == pid)
					{
						info("Target device (Product ID: %x) found.", pid);
						device = tmp_device;
						libusb_config_descriptor* config_desc;
						libusb_get_active_config_descriptor(device, &config_desc);

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
											in_endpoint = ep_desc.bEndpointAddress;
											packet_size = ep_desc.wMaxPacketSize;
											std::cout << "Found IN endpoint: 0x" << std::hex << (int)in_endpoint
													  << std::endl;
										}
										else
										{
											out_endpoint = ep_desc.bEndpointAddress;
											std::cout << "Found OUT endpoint: 0x" << std::hex << (int)out_endpoint
													  << std::endl;
										}
									}
								}
							}
						}
						// handle_device(device, vendor_id, pid);
						break; // Handle only one device at a time
					}
				}
			}
		}
	}

	libusb_free_device_list(device_list, 1);

	// Open USB device (replace with your device's Vendor ID and Product ID)
	libusb_open(device, &s_activeUsbDevice);
	if (!s_activeUsbDevice)
	{
		fprintf(stderr, "Cannot open device\n");
		libusb_exit(ctx);
		return 1;
	}

	// Detach the kernel driver if necessary
	if (libusb_kernel_driver_active(s_activeUsbDevice, 0) == 1)
	{
		r = libusb_detach_kernel_driver(s_activeUsbDevice, 0);
		if (r < 0)
		{
			fprintf(stderr, "Cannot detach kernel driver: %s\n", libusb_error_name(r));
			libusb_close(s_activeUsbDevice);
			libusb_exit(NULL);
			return 1;
		}
	}

	set_dtr_high(s_activeUsbDevice);

	// Claim interface 0 (replace with your interface number)
	r = libusb_claim_interface(s_activeUsbDevice, 0);
	if (r < 0)
	{
		fprintf(stderr, "Cannot claim interface: %s\n", libusb_error_name(r));
		libusb_close(s_activeUsbDevice);
		libusb_exit(NULL);
		return 1;
	}

	// Send the M115 command
	const char* command = "M409 K\"move\"\n";
	r = libusb_bulk_transfer(
		s_activeUsbDevice, out_endpoint, (unsigned char*)command, strlen(command), &actual_length, 0);
	if (r != 0)
	{
		std::cerr << "Error sending command: " << libusb_error_name(r) << std::endl;
		libusb_release_interface(s_activeUsbDevice, 0);
		libusb_close(s_activeUsbDevice);
		libusb_exit(ctx);
		return 1;
	}

	// Keep receiving data until no more data is available
	while (true)
	{
		r = libusb_bulk_transfer(s_activeUsbDevice, in_endpoint, data, packet_size, &actual_length, 1000);
		if (r == LIBUSB_ERROR_TIMEOUT)
		{
			warn("No more data received (timeout)");
			// break;
		}
		else if (r != 0)
		{
			warn("Error receiving data: %s", libusb_error_name(r));
			break;
		}
		// else if (actual_length == 0)
		// {
		// 	warn("No more data received");
		// 	break;
		// }
		else
		{
			info("Received (%d): %s", actual_length, std::string((char*)data, actual_length).c_str());
		}
	}

	// Release interface
	libusb_release_interface(s_activeUsbDevice, 0);

	// Close the device
	libusb_close(s_activeUsbDevice);

	// Deinitialize libusb
	libusb_exit(NULL);

	return 0;
}
