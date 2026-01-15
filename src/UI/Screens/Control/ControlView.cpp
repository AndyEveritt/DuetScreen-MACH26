#include "ControlView.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "i18n/i18n.h"

namespace UI
{
	ControlView::ControlView(const std::string& name, LvObj& parent)
		: View<ControlPresenter>(name, parent, layout_t(0, 0, 100, 100))
	{
		addStyle(Themes::getLvglStyles().bg_dark);
		setStylePad(0);

		m_tabs.setSize(LV_PCT(100), LV_PCT(100));

		m_moveView.setStylePad(0);
		m_temperatureView.setStylePad(0);
		m_heightmapView.setStylePad(0);
	}
} // namespace UI
