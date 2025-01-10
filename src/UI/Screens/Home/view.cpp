#include "view.h"

#include "utils/utils.h"

namespace UI
{
	HomeView::HomeView()
		: View(10, 0, 90, 100)
		, tabview(lv_tabview_create(m_cont))
		, main_tab(lv_tabview_add_tab(tabview, "Main"))
		, macros_tab(lv_tabview_add_tab(tabview, "Macros"))
		, m_label(lv_label_create(main_tab))
		, m_subView(main_tab)
	{
		lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
		lv_tabview_set_tab_bar_size(tabview, 60);
	}

	void HomeView::setHeaterData(const char* data)
	{
		Lock lock;
		lv_label_set_text(m_label, utils::format("Heater: %s", data).c_str());
	}
} // namespace UI
