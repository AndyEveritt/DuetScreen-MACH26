
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include "Comm/Communication.h"
#include "Comm/Usb.h"
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

	Comm::init();
	Comm::DUET.Init();

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
			Comm::JsonDecoder decoder;
			BYTE buffer[32768];
			size_t bufferLen = 0;

			while (1)
			{
				if (Comm::DUET.GetCommunicationType() != Comm::CommunicationType::usb)
				{
					// Longer delay if not in usb mode to not use as much CPU
					usleep(500 * 1000);
					continue;
				}

				if (!Comm::getCurrentUsbDevice().isConnected())
				{
					verbose("USB device disconnected");
					usleep(500 * 1000);
					continue;
				}
				int len = Comm::getCurrentUsbDevice().receive(buffer + bufferLen, 32768 - bufferLen);

				if (len > 0)
				{
					bufferLen += len;
					if (bufferLen >= 32768)
					{
						error("Buffer overflow");
						bufferLen = 0;
					}
				}
				else if (len < 0)
				{
					error("Error receiving data");
					bufferLen = 0;
				}
				if (buffer[bufferLen - 1] == '\n')
				{
					// Process the data
					decoder.CheckInput(buffer, bufferLen);
					bufferLen = 0;
				}
				usleep(5 * 1000);
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
