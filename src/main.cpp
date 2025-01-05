
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

#if LV_USE_OS == LV_OS_NONE
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
#if LV_USE_OS == LV_OS_NONE
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

	Model::getInstance(); // Initialize the model instance, this creates the subscribers

	Comm::init();
	Comm::DUET.Init();

	// Comm::DUET.RequestModel("move", "vn");

	// http_test();
	// usb_test();

	/*Initialize LVGL*/
	lv_init();

	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	hal_init(1024, 600);

#if LV_USE_OS == LV_OS_NONE

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
				usleep(1000 * 1000);
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
		printf("request failed!\n");
	}
	else
	{
		printf("%s\n", resp->body.c_str());
	}
}

static int usb_test()
{

	libusb_device_handle* handle;
	libusb_context* ctx = nullptr;
	int r;
	unsigned char data[64];			 // Data to send
	unsigned char received_data[64]; // Buffer to receive data
	int actual_length;

	// Initialize libusb
	r = libusb_init(NULL);
	if (r < 0)
	{
		fprintf(stderr, "Failed to initialize libusb\n");
		return 1;
	}

	// Open USB device (replace with your device's Vendor ID and Product ID)
	handle = libusb_open_device_with_vid_pid(NULL, 0x1d50, 0x60ec);
	if (!handle)
	{
		fprintf(stderr, "Cannot open device\n");
		libusb_exit(NULL);
		return 1;
	}

	// Detach the kernel driver if necessary
	if (libusb_kernel_driver_active(handle, 0) == 1)
	{
		r = libusb_detach_kernel_driver(handle, 0);
		if (r < 0)
		{
			fprintf(stderr, "Cannot detach kernel driver: %s\n", libusb_error_name(r));
			libusb_close(handle);
			libusb_exit(NULL);
			return 1;
		}
	}

	// Claim interface 0 (replace with your interface number)
	r = libusb_claim_interface(handle, 0);
	if (r < 0)
	{
		fprintf(stderr, "Cannot claim interface: %s\n", libusb_error_name(r));
		libusb_close(handle);
		libusb_exit(NULL);
		return 1;
	}

	// Send the M115 command
	const char* command = "M409 K\"move\"\n";
	r = libusb_bulk_transfer(
		handle, (2 | LIBUSB_ENDPOINT_OUT), (unsigned char*)command, strlen(command), &actual_length, 0);
	if (r != 0)
	{
		std::cerr << "Error sending command: " << libusb_error_name(r) << std::endl;
		libusb_release_interface(handle, 0);
		libusb_close(handle);
		libusb_exit(ctx);
		return 1;
	}

	// Keep receiving data until no more data is available
	while (true)
	{
		r = libusb_bulk_transfer(handle, (1 | LIBUSB_ENDPOINT_IN), data, sizeof(data), &actual_length, 1000);
		if (r == LIBUSB_ERROR_TIMEOUT)
		{
			std::cerr << "No more data received (timeout)" << std::endl;
			break;
		}
		else if (r != 0)
		{
			std::cerr << "Error receiving data: " << libusb_error_name(r) << std::endl;
			break;
		}
		// else if (actual_length == 0)
		// {
		// 	std::cerr << "No more data received" << std::endl;
		// 	break;
		// }
		else
		{
			std::cout << "Received (" << actual_length << "): " << std::string((char*)data, actual_length) << std::endl;
		}
	}

	// Release interface
	libusb_release_interface(handle, 0);

	// Close the device
	libusb_close(handle);

	// Deinitialize libusb
	libusb_exit(NULL);

	return 0;
}
