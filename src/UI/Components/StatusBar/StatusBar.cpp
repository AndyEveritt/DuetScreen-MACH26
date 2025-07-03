#include "StatusBar.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	StatusBar::StatusBar(lv_obj_t* parent)
		: View(lv_obj_create, "status_bar", parent)
		, m_duetStatus(lv_label_create(getRoot()))
		, m_duetName(lv_label_create(getRoot()))
		, m_time(lv_label_create(getRoot()))
	{
		activate();

		UI_LOCK();
		// setFlexFlow(LV_FLEX_FLOW_ROW);
		// setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		setSize(LV_PCT(100), LV_SIZE_CONTENT);

		lv_obj_set_align(m_duetStatus, LV_ALIGN_LEFT_MID);
		lv_obj_set_align(m_duetName, LV_ALIGN_CENTER);
		lv_obj_set_align(m_time, LV_ALIGN_RIGHT_MID);
	}

	void StatusBar::setDuetName(const std::string& name)
	{
		UI_LOCK();
		lv_label_set_text(m_duetName, name.c_str());
	}

	void StatusBar::setDuetStatus(const std::string& status)
	{
		UI_LOCK();
		lv_label_set_text(m_duetStatus, status.c_str());
	}

	void StatusBar::setTime(const std::string& time)
	{
		UI_LOCK();
		lv_label_set_text(m_time, time.c_str());
	}
} // namespace UI
