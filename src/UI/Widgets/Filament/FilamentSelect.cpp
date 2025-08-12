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

			lv_anim_t anim;
			lv_anim_init(&anim);
			lv_anim_set_duration(&anim, 300);
			lv_anim_set_var(&anim, &control.m_widget);
			lv_anim_set_values(&anim, control.m_widget.m_cont.getHeight(), 0);
			lv_anim_set_exec_cb(&anim,
								[](void* var, int32_t value)
								{
									auto& widget = *static_cast<FilamentSelect*>(var);
									widget.m_filamentOptions.setY(value);
								});

			lv_anim_start(&anim);
		}

		LvLabel m_toolName;
		Button m_filament;
		FilamentSelect& m_widget;
	};

	FilamentSelect::FilamentSelect(const std::string& name, lv_obj_t* parent, lv_obj_t* messageBoxParent)
		: View(name, parent)
		, m_header("header", getRoot())
		, m_cont("cont", getRoot())
		, m_toolList("tool_list", m_cont)
		, m_confirmation("confirmation", messageBoxParent, layout_t(0, 0, 50, 70))
		, m_filamentOptions("filament_options", m_confirmation)
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_header.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_cont.setWidth(LV_PCT(100));
		m_cont.setFlexGrow(1);
		m_cont.setFlag(LV_OBJ_FLAG_SCROLLABLE, false);
		m_toolList.setSize(LV_PCT(100), LV_PCT(100));
		m_filamentOptions.setSize(LV_PCT(100), LV_PCT(100));
		m_filamentOptions.setFlag(LV_OBJ_FLAG_IGNORE_LAYOUT, true);
		// m_filamentOptions.setAlign(LV_ALIGN_OUT_BOTTOM_MID, 0, 0); // Position outside the visible area initially

		m_toolList.setListFlow(LV_FLEX_FLOW_COLUMN);
		m_toolList.getListContainer().setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_filamentOptions.setListFlow(LV_FLEX_FLOW_ROW_WRAP);
		m_filamentOptions.getListContainer().setFlexAlign(
			LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_toolList.setListGrow(1);

		m_header.setText(_("filament_management_header"));
		m_toolList.setTitle(_("select_tool"));
		m_filamentOptions.setTitle(_("select_filament"));
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
			control.m_confirmation.setText(fmt::format(fmt::runtime(_("confirm_filament_change")), selectedFilament));
			control.m_confirmation.setOkCallback([presenter, selectedFilament]()
												 { presenter->setFilament(selectedFilament); });
			control.m_confirmation.show(true);
		}
	}

	void FilamentSelect::onShow()
	{
		m_filamentOptions.setY(m_cont.getHeight());
	}
} // namespace UI
