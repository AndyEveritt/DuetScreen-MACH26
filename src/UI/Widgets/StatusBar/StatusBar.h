#pragma once

#include "StatusBarPresenter.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/LVGL/LvSpan.h"
#include "UI/Core/View.h"

namespace UI
{
	class StatusBar : public View<StatusBarPresenter>
	{
	  public:
		StatusBar(LvObj& parent);

		void setDuetName(std::string_view name);
		void setDuetStatus(std::string_view status);
		void setTime(std::string_view time);
		void setTemperatures(std::string_view temperatures);

		auto& getTemperatures() { return m_temperatures; }

	  private:
		void updateTemperaturesPosition();

		LvLabel m_duetStatus{"status", getRoot()};
		LvLabel m_duetName{"name", getRoot()};
		LvLabel m_time{"time", getRoot()};
		LvContainer m_temperatureContainer{"temp_container", getRoot()};
		LvSpanGroup m_temperatures{"temperatures", m_temperatureContainer};
	};
} // namespace UI