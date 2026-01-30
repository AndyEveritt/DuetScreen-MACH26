/*
 * BabyStep.cpp
 *
 *  Created on: 2025-09-25
 *      Author: Andy Everitt
 */

#include "BabyStep.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"
#include "utils/StorageHelper.h"
#include "utils/UnitSystem.h"

namespace UI
{
	BabyStep::BabyStep(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		ZoneScoped;
		UI_LOCK();

		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_header.setText(_("babystep.header"));

		m_buttonPanel.setFlexGrow(1);
		m_buttonPanel.setWidth(LV_PCT(100));
		// m_buttonPanel.setMinWidth(LV_SIZE_CONTENT);

		setBabyStepValue(0);
		m_buttonPanel.setIncrementIcon("babystep_increment.png");
		m_buttonPanel.setDecrementIcon("babystep_decrement.png");
		m_buttonPanel.setValueLabelFmt("{:g} mm");
		m_buttonPanel.setValueChangeCallback([this](float change) { m_presenter->babystep(change); });
		m_buttonPanel.setResetCallback([this]() { m_presenter->resetBabystep(); });
		m_buttonPanel.setUpdatedValuesCallback(
			[this](const std::vector<float>& values)
			{ StorageHelper::setData<std::vector<float>>(ID_BABYSTEP_AMOUNT, values); });
		m_buttonPanel.setMinValue(0.001f);
		m_buttonPanel.setMaxValue(1.0f);

		m_buttonPanel.getResetButton().addStyle(Themes::getLvglStyles().actionBtn);
		m_buttonPanel.getIncrementButton().addStyle(Themes::getLvglStyles().actionBtn);
		m_buttonPanel.getDecrementButton().addStyle(Themes::getLvglStyles().actionBtn);
	}

	void BabyStep::setBabyStepValue(float value)
	{
		ZoneScoped;
		m_buttonPanel.setResetLabel(_("babystep.reset", value));
	}

	void BabyStep::onShow()
	{
		ZoneScoped;
		const auto values = StorageHelper::getData<std::vector<float>>(ID_BABYSTEP_AMOUNT, {0.01f, 0.05f});
		m_buttonPanel.setIncrementValues(values);
	}
} // namespace UI
