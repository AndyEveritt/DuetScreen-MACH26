/*
 * MultiValueSelector.cpp
 *
 *  Created on: 2025-11-16
 *      Author: Andy Everitt
 */

#include "MultiValueSelector.h"
#include "Debug.h"

namespace UI
{
	MultiValueSelector::MultiValueSelector(const std::string& name, LvObj& parent)
		: LvContainer(name, parent)
	{
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_topRow.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_topRow.setSize(LV_PCT(100), LV_PCT(50));
		m_topRow.setMinHeight(LV_SIZE_CONTENT);

		m_decrement.setIcon("decrement.png");
		m_increment.setIcon("increment.png");

		m_valueDisplay.setHeight(LV_PCT(100));
		m_valueDisplay.setMinHeight(LV_SIZE_CONTENT);
		m_valueDisplay.setFlexGrow(1);

		m_values.setWidth(LV_PCT(100));
		m_values.setFlexGrow(1);
		m_values.setMinHeight(LV_SIZE_CONTENT);
		m_values.setListFlow(LV_FLEX_FLOW_ROW);
		m_values.getListContainer().setWidth(LV_PCT(100));
		m_values.getListContainer().setFlexGrow(1);
		m_values.getListContainer().setMinHeight(LV_SIZE_CONTENT);
	}

	void MultiValueSelector::setValues(const std::vector<float>& values)
	{
		UI_LOCK();
		m_currentValues = values;

		m_values.setItemCount(values.size(),
							  [](size_t index, LvObj& parent)
							  {
								  auto btn = std::make_unique<Button>(std::to_string(index), parent);
								  btn->setHeight(LV_PCT(100));
								  btn->setMinHeight(LV_SIZE_CONTENT);
								  btn->setFlexGrow(1);
								  btn->setMinWidth(LV_SIZE_CONTENT);
								  return btn;
							  });

		m_values.iterateListItems([&values](size_t index, Button& btn)
								  { btn.setText(std::to_string(values.at(index))); });
	}
} // namespace UI
