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
#include "utils/GpioHelper.h"
#include "utils/StorageHelper.h"
#include "utils/UpgradeHelper.h"
#include <filesystem>
#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
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
void lvgl_log_cb(lv_log_level_t level, const char* buf);
static lv_display_t* hal_init(int32_t w, int32_t h);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void lvgl_testing()
{
	lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW);

	lv_obj_t* cont = lv_obj_create(lv_screen_active());
	lv_obj_t* btn = lv_button_create(cont);
	lv_label_create(btn);

#if 0
	lv_obj_t* btn2 = lv_button_create(cont);
	lv_label_create(btn2);
#endif

	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_size(cont, 100, LV_PCT(100));
	lv_obj_set_size(btn, 1, LV_SIZE_CONTENT);
	lv_obj_set_style_min_width(btn, LV_PCT(50), LV_PART_MAIN);

	lv_obj_t* cont2 = lv_obj_create(lv_screen_active());
	lv_obj_t* btn2 = lv_button_create(cont2);
	lv_label_create(btn2);

	lv_obj_set_flex_flow(cont2, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_size(cont2, LV_SIZE_CONTENT, LV_PCT(100));
	lv_obj_set_size(btn2, LV_PCT(100), LV_SIZE_CONTENT);
	lv_obj_set_style_min_width(btn2, LV_SIZE_CONTENT, LV_PART_MAIN);

	lv_obj_t* bm = lv_buttonmatrix_create(lv_screen_active());
	lv_obj_set_size(bm, 100, LV_PCT(100));
}

/**********************
 *      VARIABLES
 **********************/

int main(int argc, char** argv)
{
	(void)argc; /*Unused*/
	(void)argv; /*Unused*/

	lv_init();

	StorageHelper::load();
	Log::Init();

	/*Initialize LVGL*/
	lv_log_register_print_cb(lvgl_log_cb);
	lv_i18n_init(lv_i18n_language_pack);
	lv_i18n_set_locale(StorageHelper::getData<std::string>(ID_SYS_LANG_CODE_KEY, DEFAULT_LANGUAGE_CODE).c_str());

	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	lv_display_t* display = hal_init(1024, 600);

	UI::Themes::init(display);

	lvgl_testing();

	while (1)
	{
		{
			// UI_LOCK();
			// LOG_DBG("Updating UI");
			lv_timer_handler();
		}
		usleep(5 * 1000); // Sleep for 5 milliseconds
	}

	return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void lvgl_log_cb(lv_log_level_t level, const char* buf)
{
	switch (level)
	{
	case LV_LOG_LEVEL_TRACE:
		LOG_VERBOSE("{:s}", buf);
		break;
	case LV_LOG_LEVEL_INFO:
		LOG_INFO("{:s}", buf);
		break;
	case LV_LOG_LEVEL_WARN:
		LOG_WARN("{:s}", buf);
		break;
	case LV_LOG_LEVEL_ERROR:
		LOG_ERROR("{:s}", buf);
		break;
	default:
		break;
	}
}

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
	LOG_INFO("Initialising display");
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
