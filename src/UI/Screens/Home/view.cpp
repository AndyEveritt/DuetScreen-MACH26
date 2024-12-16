#include "view.h"

#define MACROS_SYMBOL "\xF3\xB1\xB2\x83"
#define CONSOLE_SYMBOL "\xF3\xB0\x86\x8D"
#define TUNE_SYMBOL "\xF3\xB1\x95\x82"
#define HOME_SYMBOL "\xF3\xB0\x8B\x9C"
#define SETTING_SYMBOL "\xF3\xB0\x92\x93"

namespace UI
{
	HomeView::HomeView()
		: View(10, 0, 90, 100)
		, tabview(lv_tabview_create(m_cont))
		, main_tab(lv_tabview_add_tab(tabview, HOME_SYMBOL))
		, macros_tab(lv_tabview_add_tab(tabview, MACROS_SYMBOL))
		, m_subView(main_tab)
	{
		lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
		lv_tabview_set_tab_bar_size(tabview, 60);
	}
} // namespace UI
