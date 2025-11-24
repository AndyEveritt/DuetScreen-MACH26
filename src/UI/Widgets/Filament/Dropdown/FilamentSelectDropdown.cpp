/*
 * FilamentSelectDropdown.cpp
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#include "FilamentSelectDropdown.h"
#include "Debug.h"
#include "i18n/i18n.h"

namespace UI
{
	FilamentSelectDropdown::FilamentSelectDropdown(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_ROW);

		m_selection.setHeight(LV_SIZE_CONTENT);
		m_selection.setFlexGrow(1);
		m_selection.setMinWidth(LV_SIZE_CONTENT);
		m_changeBtn.setSize(LV_SIZE_CONTENT, LV_PCT(100));
		m_unloadBtn.setSize(LV_SIZE_CONTENT, LV_PCT(100));

		m_selection.setLabel(_("filament.select"));
		m_changeBtn.setText(_("filament.change"));
		m_unloadBtn.setText(_("filament.unload"));

		setFilamentSelected("", true);

		m_selection.addEventCallback(onFilamentSelectEvent, LV_EVENT_VALUE_CHANGED, this);
		m_changeBtn.addClickedCallback(onFilamentChangeEvent, this);
		m_unloadBtn.addClickedCallback(onFilamentUnloadEvent, this);

		m_changeBtn.addStyle(Themes::getLvglStyles().actionBtn);
		m_unloadBtn.addStyle(Themes::getLvglStyles().actionBtn);
	}

	void FilamentSelectDropdown::setToolIndex(size_t index)
	{
		UI_LOCK();
		LOG_DBG("Setting tool index to {} for {}", index, getName());
		getPresenter()->setSelectedToolBySlot(index);
	}

	void FilamentSelectDropdown::showHint(bool show)
	{
		m_selection.getLabel().setVisible(show);
	}

	void FilamentSelectDropdown::setFilamentDisabled(bool disabled)
	{
		if (disabled == hasState(LV_STATE_DISABLED))
		{
			return;
		}

		setState(LV_STATE_DISABLED, disabled, true);
		m_selection.setOptions(disabled ? std::vector<std::string>() : m_filamentOptions);
		// m_selection.setText(disabled ? "" : m_loadedFilament);
	}

	void FilamentSelectDropdown::setFilamentOptions(const std::vector<std::string>& options)
	{
		UI_LOCK();
		LOG_DBG("Setting filament options for {}", getName());
		m_filamentOptions = options;
		m_selection.setOptions(options);
	}

	void FilamentSelectDropdown::setFilamentSelected(const std::string& filament, bool forceUpdate)
	{
		UI_LOCK();
		LOG_DBG("Setting selected filament to '{}' for {}", filament, getName());

		if (filament == m_loadedFilament && !forceUpdate)
		{
			return;
		}

		m_selection.setSelected(filament);
		m_selection.setText(filament.empty() ? _("filament.not_loaded") : filament);
		m_loadedFilament = filament;
		m_changeBtn.hide();
		m_unloadBtn.setDisabled(filament.empty());
		m_changeBtn.setText(filament.empty() ? _("filament.load") : _("filament.change"));
		m_selection.invalidate(); // lvgl bug? This shouldn't be necessary
	}

	void FilamentSelectDropdown::onFilamentSelectEvent(lv_event_t* event)
	{
		UI_LOCK();
		auto control = static_cast<FilamentSelectDropdown*>(lv_event_get_user_data(event));

		std::string selected_filament = control->m_selection.getSelectedString();
		control->m_selection.setText(selected_filament);
		control->m_selection.invalidate(); // lvgl bug? This shouldn't be necessary

		control->m_changeBtn.setVisible(selected_filament != control->m_loadedFilament);
	}

	void FilamentSelectDropdown::onFilamentChangeEvent(lv_event_t* event)
	{
		UI_LOCK();
		auto control = static_cast<FilamentSelectDropdown*>(lv_event_get_user_data(event));

		assert(control);

		std::string selected = control->m_selection.getSelectedString();
		LOG_DBG("Calling filament callback for '{}' in {}", selected, control->getName());
		control->getPresenter()->setFilament(selected);
	}

	void FilamentSelectDropdown::onFilamentUnloadEvent(lv_event_t* event)
	{
		UI_LOCK();
		auto control = static_cast<FilamentSelectDropdown*>(lv_event_get_user_data(event));

		assert(control);

		control->getPresenter()->unloadFilament();
	}
} // namespace UI
