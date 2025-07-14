#include "TemperatureView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	TemperatureView::TemperatureView(lv_obj_t* parent)
		: View("temperature_view", parent, layout_t(0, 0, 100, 100))
		, m_tools(getName() + "_tools", getRoot())
		, m_beds(getName() + "_beds", getRoot())
		, m_chambers(getName() + "_chambers", getRoot())
		, m_numberPad(getName() + "_number_pad", getRoot(), layout_t(0, 0, 50, 70))
	{
		UI_LOCK();

		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_tools.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_beds.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_chambers.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_tools.setTitle(_("tools"));
		m_beds.setTitle(_("beds"));
		m_chambers.setTitle(_("chambers"));

		m_tools.addStyle(Themes::getLvglStyles().no_border);
		m_beds.addStyle(Themes::getLvglStyles().no_border);
		m_chambers.addStyle(Themes::getLvglStyles().no_border);
		// m_tools.addListStyle(Themes::getLvglStyles().no_border);
		// m_beds.addListStyle(Themes::getLvglStyles().no_border);
		// m_chambers.addListStyle(Themes::getLvglStyles().no_border);
	}

	void TemperatureView::onShow()
	{
		m_numberPad.hide();
	}

	void TemperatureView::onHide() {}
} // namespace UI
