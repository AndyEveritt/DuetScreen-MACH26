#include "TemperatureView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	TemperatureView::TemperatureView(const std::string& name, LvObj& parent)
		: View(name, parent, layout_t(0, 0, 100, 100))
	{
		UI_LOCK();

		addStyle(Themes::getLvglStyles().bg_dark);
		m_temperatureCont.addStyle(Themes::getLvglStyles().card);
		m_extruderControl.addStyle(Themes::getLvglStyles().card);

		setFlexFlow(LV_FLEX_FLOW_ROW);

		m_temperatureCont.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_temperatureCont.setHeight(LV_PCT(100));
		m_temperatureCont.setFlexGrow(2);

		m_controlCont.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_controlCont.setHeight(LV_PCT(100));
		m_controlCont.setFlexGrow(1);
		m_controlCont.setStylePad(0);

		m_selectedToolLabel.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_selectedToolLabel.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);

		m_extruderControl.setWidth(LV_PCT(100));
		m_extruderControl.setFlexGrow(1);
		m_extruderControl.setNumberPad(&m_numberPad);

		m_tools.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_beds.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_chambers.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_tools.setTitle(_("temperature.tools"));
		m_beds.setTitle(_("temperature.beds"));
		m_chambers.setTitle(_("temperature.chambers"));
	}

	void TemperatureView::setSelectedToolName(std::string_view tool_name)
	{
		m_selectedToolLabel.setText(_("temperature.selected_tool_label", tool_name));
	}

	void TemperatureView::onShow()
	{
		m_numberPad.close();
	}

	void TemperatureView::onHide() {}
} // namespace UI
