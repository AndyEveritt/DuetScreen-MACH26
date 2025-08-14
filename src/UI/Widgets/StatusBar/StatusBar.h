#pragma once

#include "StatusBarPresenter.h"
#include "UI/Core/View.h"

namespace UI
{
	class StatusBar : public View<StatusBarPresenter>
	{
	  public:
		StatusBar(lv_obj_t* parent);

		void setDuetName(const std::string& name);
		void setDuetStatus(const std::string& status);
		void setTime(const std::string& time);

	  private:
		lv_obj_t* m_duetName;
		lv_obj_t* m_duetStatus;
		lv_obj_t* m_time;
	};
} // namespace UI