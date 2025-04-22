
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
#include "Hardware/Reset.h"
#include "Hardware/Usb.h"
#include "UI/Screens/Home/HomeView.h"
#include "UI/Styles/Styles.h"
#include "glob.h"
#include "hv/requests.h"
#include "lv_i18n/lv_i18n.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/core/lv_global.h"
#include "utils/DisplayHelper.h"
#include "utils/StorageHelper.h"
#include "utils/UpgradeHelper.h"
#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if LV_USE_OS == LV_OS_PTHREAD
#  include <pthread.h>
#elif LV_USE_OS == LV_OS_FREERTOS
#  include "freertos_main.h"
#endif

#if T113
#elif SIMULATION
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
static pthread_t s_responseThread;
static pthread_t s_requestThread;
static pthread_t s_thumbnailThread;

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

	// LVGL thread needs access to both the UI and Model mutexes. It is the only thread allowed to take both otherwise
	// deadlocks can occur
	DeadlockDetector::getInstance().allowThreadToTakeMultipleLocks(std::this_thread::get_id(), true);

	// Initialise
	StorageHelper::load();
	SetDebugLevel(StorageHelper::getData(ID_DEBUG_LEVEL, DebugLevel::Info));

	/*Initialize LVGL*/
	lv_init();
	lv_i18n_init(lv_i18n_language_pack);
	lv_i18n_set_locale(StorageHelper::getData<std::string>(ID_SYS_LANG_CODE_KEY, DEFAULT_LANGUAGE_CODE).c_str());

	Model::get(); // Initialize the model instance, this creates the subscribers

	Comm::init();
	Comm::DUET.Init();

	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	lv_display_t* display = hal_init(1024, 600);

	DisplayHelper::setBrightness(StorageHelper::getData(ID_SYS_BRIGHTNESS_KEY, 100u));
	UI::Styles::instance().init(display);

	UI::HomeView home = UI::HomeView::instance();
	home.show();

	USB::UsbMonitor::getInstance().registerCallback(
		[](const std::string& path, bool mounted)
		{
			if (mounted)
			{
				info("USB drive mounted: %s", path.c_str());
				std::string upgradeFilePath = path + "/DuetScreen.tar.gz";
				if (!UpgradeHelper::upgradeFromUSB(upgradeFilePath))
				{
					error("Upgrade from USB failed");
				}
			}
		});
	USB::UsbMonitor::getInstance().startMonitoring();

	// Create a thread to handle requesting data from Duet
#if MULTITHREADED
	pthread_create(
		&s_requestThread,
		NULL,
		[](void*) -> void*
		{
			while (1)
			{
				// Request next section of the OM
				Model::get().requestNewData();
				usleep(Comm::DUET.GetScaledPollInterval() * 1000);
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
				useconds_t delay = Model::get().receiveNewUsbData();
				usleep(delay);
			}
			return nullptr;
		},
		NULL);

	pthread_create(
		&s_thumbnailThread,
		NULL,
		[](void*) -> void*
		{
			while (1)
			{
				FILEINFO_CACHE->Spin();
				usleep(50 * 1000);
			}
			return nullptr;
		},
		NULL);
#endif

	// Screensaver task
	DisplayHelper::setScreenSaverBrightness(0);
	lv_timer_t* screensaverTimer = lv_timer_create(
		[](lv_timer_t* timer)
		{
			static bool screensaverEnabled = false;
			uint32_t inactiveTime = lv_display_get_inactive_time(NULL);
			uint32_t timeout = StorageHelper::getData(ID_SCREENSAVER_TIMEOUT, DEFAULT_SCREEN_TIMEOUT);
			if (timeout > 0 && inactiveTime > timeout)
			{
				if (!screensaverEnabled)
				{
					info("Screensaver timeout reached");
					DisplayHelper::enableScreenSaver(true);
					screensaverEnabled = true;
				}
			}
			else
			{
				if (screensaverEnabled)
				{
					info("Screensaver timeout cancelled");
					DisplayHelper::enableScreenSaver(false);
					screensaverEnabled = false;
				}
			}
		},
		100, // Timer period in milliseconds
		NULL);

	while (1)
	{
		{
			UI_LOCK();
			lv_timer_handler();
		}
		usleep(5 * 1000); // Sleep for 5 milliseconds
	}

	return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static const char* getenv_default(const char* name, const char* dflt)
{
	return getenv(name) ?: dflt;
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t* hal_init(int32_t w, int32_t h)
{
	info("Initialising display");
#if LV_USE_LINUX_FBDEV
	const char* device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
	lv_display_t* disp = lv_linux_fbdev_create();
	lv_display_set_resolution(disp, w, h);

#  if LV_USE_EVDEV
	const char* input_device = getenv_default("LV_LINUX_EVDEV_POINTER_DEVICE", "/dev/input/event1");
	lv_indev_t* touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, input_device);
	lv_indev_set_display(touch, disp);
#  endif

	lv_linux_fbdev_set_file(disp, device);

#elif LV_USE_SDL

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

#else
#  error Unsupported configuration
#endif
	return disp;
}
