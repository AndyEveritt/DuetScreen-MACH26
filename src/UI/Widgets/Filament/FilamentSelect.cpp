/*
 * FilamentSelect.cpp
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#include "FilamentSelect.h"
#include "Debug.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	class FilamentSelect::ToolItem : public ListItem
	{
	  public:
		ToolItem(const size_t index, lv_obj_t* parent, FilamentSelect& widget)
			: ListItem(index, parent)
			, m_toolName("tool", getRoot())
			, m_filament("filament", getRoot())
			, m_widget(widget)
		{
			UI_LOCK();
			setFlexFlow(LV_FLEX_FLOW_ROW);
			setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
			setSize(LV_PCT(100), LV_SIZE_CONTENT);

			m_toolName.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			m_filament.setHeight(LV_SIZE_CONTENT);
			m_filament.setFlexGrow(1);
			m_filament.setUserData(&m_filament);
			m_filament.addClickedCallback(onToolSelectEvent, this);

			m_filament.addStyle(Themes::getLvglStyles().actionBtn, 0);
		}

		void setToolName(std::string_view name) { m_toolName.setText(name); }

		void setFilamentName(std::string_view filament)
		{
			UI_LOCK();
			m_filament.setText(filament);
		}

	  private:
		static void onToolSelectEvent(lv_event_t* e)
		{
			auto& control = *static_cast<ToolItem*>(lv_event_get_user_data(e));
			auto& btn = *static_cast<Button*>(lv_obj_get_user_data((lv_obj_t*)lv_event_get_target(e)));

			auto presenter = control.m_widget.getPresenter();
			presenter->setSelectedToolBySlot(control.getIndex());
		}

		LvLabel m_toolName;
		Button m_filament;
		FilamentSelect& m_widget;
	};

	FilamentSelect::FilamentSelect(const std::string& name, lv_obj_t* parent)
		: View(name, parent)
		, m_header("header", getRoot())
		, m_toolList("tool_list", getRoot())
		, m_filamentOptions("filament_options", getRoot())
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_header.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_toolList.setSize(LV_PCT(100), 0);
		m_toolList.setFlexGrow(1);
		m_filamentOptions.setSize(LV_PCT(100), 0);
		m_filamentOptions.setFlexGrow(1);

		m_toolList.setListFlow(LV_FLEX_FLOW_COLUMN);
		m_toolList.getListContainer().setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_filamentOptions.setListFlow(LV_FLEX_FLOW_ROW_WRAP);
		m_filamentOptions.getListContainer().setFlexAlign(
			LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_toolList.setListGrow(1);

		m_header.setText(_("filament_management_header"));
		m_toolList.setTitle(_("select_tool"));
		m_filamentOptions.setTitle(_("select_filament"));

		m_toolList.addStyle(Themes::getLvglStyles().no_border);
		m_filamentOptions.addStyle(Themes::getLvglStyles().no_border);
	}

	void FilamentSelect::setToolCount(size_t count)
	{
		UI_LOCK();
		m_toolList.setItemCount(count, *this);
	}

	void FilamentSelect::setToolData(size_t index, std::string_view toolName, std::string_view filamentName)
	{
		UI_LOCK();
		if (index >= m_toolList.getItemCount())
		{
			LOG_WARN("Index {} out of bounds for tool data in {}", index, getName());
			return;
		}

		auto item = m_toolList.getItem(index);
		if (!item)
		{
			LOG_ERROR("Failed to get tool item at index {} in {}", index, getName());
			return;
		}
		item->setToolName(toolName);
		item->setFilamentName(filamentName);
	}

	void FilamentSelect::setFilamentOptions(const std::vector<std::string>& options)
	{
		UI_LOCK();
		LOG_DBG("Setting filament options for {}", getName());
		m_filamentOptions.clear();
		m_filamentOptions.setItemCount(options.size(),
									   [this, &options](size_t index, lv_obj_t* parent) -> std::shared_ptr<Button>
									   {
										   auto btn = std::make_shared<Button>(fmt::format("{}", index), parent);
										   btn->setText(options[index]);
										   btn->setFlexGrow(1);
										   btn->setUserData(btn.get());
										   btn->addClickedCallback(onFilamentOptionClicked, this);
										   return btn;
									   });
	}

	void FilamentSelect::showToolSelect(bool show)
	{
		m_toolList.setVisible(show);
	}

	void FilamentSelect::setSelectedFilament(std::string_view filamentName)
	{
		LOG_DBG("Setting selected filament to {}", filamentName);
		for (size_t i = 0; i < m_filamentOptions.getItemCount(); i++)
		{
			auto item = m_filamentOptions.getItem(i);
			if (!item)
			{
				LOG_ERROR("Failed to get filament option item at index {} in {}", i, getName());
				continue;
			}
			item->setChecked(item->getLabel().getText() == filamentName);
		}
	}

	void FilamentSelect::onFilamentOptionClicked(lv_event_t* e)
	{
		auto& control = *static_cast<FilamentSelect*>(lv_event_get_user_data(e));
		auto& btn = *static_cast<Button*>(lv_obj_get_user_data((lv_obj_t*)lv_event_get_target(e)));

		auto presenter = control.getPresenter();

		std::string_view selectedFilament = btn.getText();
		if (!control.m_confirmation)
		{
			presenter->setFilament(selectedFilament);
		}
		else
		{
			control.m_confirmation->setText(fmt::format(fmt::runtime(_("confirm_filament_change")), selectedFilament));
			control.m_confirmation->setOkCallback([presenter, selectedFilament]()
												  { presenter->setFilament(selectedFilament); });
			control.m_confirmation->show(true);
		}
	}
} // namespace UI
