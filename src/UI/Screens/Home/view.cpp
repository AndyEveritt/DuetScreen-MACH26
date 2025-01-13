#include "view.h"

#include "Debug.h"
#include "lv_i18n/lv_i18n.h"
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
		, m_btn("button", m_cont, "Click me")
		, m_btn2("button2", m_cont, "Click me")
	{
		lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
		lv_tabview_set_tab_bar_size(tabview, 60);

		m_btn.setX(40);
		m_btn.setWidth(10);
		m_btn.setY(0);
		lv_obj_get_style_pad_bottom(m_btn.getObj(), LV_PART_MAIN);
		m_btn2.setX(50);
		m_btn2.setY(0);

		m_btn.setCallback(btnCallback, LV_EVENT_CLICKED, this);

		lv_obj_set_style_border_width(m_btn.getObj(), 0, 0);

		// lv_obj_set_size(m_btn.getObj(), LV_SIZE_CONTENT, LV_SIZE_CONTENT);
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
		verbose("Button clicked");
	}
} // namespace UI
