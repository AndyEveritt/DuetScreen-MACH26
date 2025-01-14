#include "view.h"

#include "Debug.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/utils.h"

namespace UI
{
	HomeView::HomeView()
		: View("HomeView", layout_t(10, 0, 90, 100))
		, tabview(lv_tabview_create(m_cont))
		, main_tab(lv_tabview_add_tab(tabview, "Main"))
		, macros_tab(lv_tabview_add_tab(tabview, "Macros"))
		, m_label(lv_label_create(main_tab))
		, m_subView(main_tab)
		, m_btn("button", main_tab, "Click me", layout_t(0, 10, 50, 10))
		, m_btn2("button2", main_tab, "Button 2", layout_t(10, 30, 10, 10))
	{
		lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
		lv_tabview_set_tab_bar_size(tabview, 60);

		lv_obj_get_style_pad_bottom(m_btn.getCont(), LV_PART_MAIN);

		m_btn.setCallback(btnCallback, LV_EVENT_CLICKED, this);

		layout_t layout = m_btn.getLayout();

		lv_obj_set_style_border_width(m_btn.getCont(), 0, 0);

		// lv_obj_set_size(m_btn.getCont(), LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	}

	void HomeView::setHeaterData(const char* data)
	{
		Lock lock;
		lv_label_set_text(m_label, utils::format(_("heater"), data).c_str());
	}

	void HomeView::btnCallback(lv_event_t* e)
	{
		lv_obj_t* obj = static_cast<lv_obj_t*>(lv_event_get_target(e));
		HomeView* view = static_cast<HomeView*>(lv_event_get_user_data(e));

		Button* btn = static_cast<Button*>(lv_obj_get_user_data(obj));
		verbose("Button '%s' clicked", btn->getName());
	}
} // namespace UI
