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
		, m_temperatureCont("temperature_cont", getRoot())
		, m_tools("tools", m_temperatureCont)
		, m_beds("beds", m_temperatureCont)
		, m_chambers("chambers", m_temperatureCont)
		, m_filamentSelect("filament_select", getRoot())
		, m_numberPad("number_pad", getRoot(), layout_t(0, 0, 50, 70))
	{
		UI_LOCK();

		setFlexFlow(LV_FLEX_FLOW_ROW);

		m_temperatureCont.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_temperatureCont.setHeight(LV_PCT(100));
		m_temperatureCont.setFlexGrow(2);

		m_filamentSelect.setHeight(LV_PCT(100));
		m_filamentSelect.setFlexGrow(1);

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
		m_filamentSelect.show();
	}

	void TemperatureView::onHide() {}
} // namespace UI
