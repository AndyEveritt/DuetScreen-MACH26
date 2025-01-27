
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
#include "UI/Styles/Styles.h"
#include "glob.h"
#include "hv/requests.h"
#include "lv_i18n/lv_i18n.h"
#include "lvgl/lvgl.h"
#include "utils/StorageHelper.h"
#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "UI/Screens/Home/HomeView.h"

#if LV_USE_OS == LV_OS_PTHREAD
#  include <pthread.h>
#elif LV_USE_OS == LV_OS_FREERTOS
#  include "freertos_main.h"
#endif

#if T113
#  include "lv_drivers/display/sunxifb.h"
#  include "lv_drivers/indev/evdev.h"
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

	/*Initialize LVGL*/
	lv_init();
	lv_i18n_init(lv_i18n_language_pack);
	lv_i18n_set_locale(StorageHelper::getData<std::string>(ID_SYS_LANG_CODE_KEY, DEFAULT_LANGUAGE_CODE).c_str());

	Model::get(); // Initialize the model instance, this creates the subscribers

	Comm::init();
	Comm::DUET.Init();

	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	lv_display_t* display = hal_init(1024, 600);

	UI::Styles::instance().init(display);

#if LV_USE_OS == LV_OS_PTHREAD

	// lv_demo_widgets();
	// lv_demo_benchmark();

	UI::HomeView home = UI::HomeView::instance();
	home.show();

	// Create a thread to handle requesting data from Duet
#  if MULTITHREADED
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
#  endif

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
#if T113
	lv_display_rotation_t rotated = LV_DISPLAY_ROTATION_0;

	/*Linux frame buffer device init*/
	sunxifb_init(rotated);

	/*A buffer for LittlevGL to draw the screen's content*/
	static uint32_t width, height;
	sunxifb_get_sizes(&width, &height);

	static lv_color_t* buf;
	buf = (lv_color_t*)malloc(width * height * sizeof(lv_color_t));

	if (buf == NULL)
	{
		sunxifb_exit();
		printf("malloc draw buffer fail\n");
		return 0;
	}

	lv_display_t* disp = lv_display_create(width, height);
	if (disp == NULL)
	{
		fatal("Failed to create display");
		exit(1);
	}
	lv_display_set_flush_cb(disp, sunxifb_flush);
	lv_display_set_buffers(disp, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_DIRECT);

	/*Initialize and register a display driver*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.draw_buf = &disp_buf;
	disp_drv.flush_cb = sunxifb_flush;
	disp_drv.hor_res = width;
	disp_drv.ver_res = height;
	disp_drv.rotated = rotated;
	lv_disp_drv_register(&disp_drv);

	evdev_init();
	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);			/*Basic initialization*/
	indev_drv.type = LV_INDEV_TYPE_POINTER; /*See below.*/
	indev_drv.read_cb = evdev_read;			/*See below.*/
	/*Register the driver in LVGL and save the created input device object*/
	lv_indev_t* evdev_indev = lv_indev_drv_register(&indev_drv);
#elif SIMULATION
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
#  error "No hardware abstraction layer defined"
#endif

	return disp;
}
