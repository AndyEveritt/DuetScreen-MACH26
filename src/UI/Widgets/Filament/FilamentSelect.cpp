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
		ToolItem(const size_t index, lv_obj_t* parent)
			: ListItem("filament_tool_item", index, parent)
			, m_toolName(getName() + "_tool", getRoot())
			, m_filament(getName() + "filament", getRoot())
		{
			UI_LOCK();
			setFlexFlow(LV_FLEX_FLOW_ROW);
			setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
			setSize(LV_PCT(100), LV_SIZE_CONTENT);

			m_toolName.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			m_filament.setHeight(LV_SIZE_CONTENT);
			m_filament.setFlexGrow(1);

			m_filament.addStyle(Themes::getLvglStyles().actionBtn, 0);
		}

		void setToolName(std::string_view name) { m_toolName.setText(name); }

		void setFilamentName(std::string_view filament)
		{
			UI_LOCK();
			m_filament.setText(filament);
		}

	  private:
		LvLabel m_toolName;
		Button m_filament;
	};

	FilamentSelect::FilamentSelect(const std::string& name, lv_obj_t* parent)
		: View(name, parent)
		, m_header(name + "_header", getRoot())
		, m_toolList(name + "_tool_list", getRoot())
		, m_filamentOptions(name + "_filament_options", getRoot())
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_header.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_toolList.setWidth(LV_PCT(100));
		m_toolList.setFlexGrow(1);
		m_filamentOptions.setWidth(LV_PCT(100));
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
		m_toolList.setItemCount(count,
								[this](size_t index, lv_obj_t* parent) -> std::shared_ptr<ToolItem>
								{ return std::make_shared<ToolItem>(index, parent); });
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
		m_filamentOptions.setItemCount(options.size(),
									   [this, &options](size_t index, lv_obj_t* parent) -> std::shared_ptr<Button>
									   {
										   auto btn = std::make_shared<Button>(
											   fmt::format("{}_filament_option_{}", getName(), index), parent);
										   btn->setText(options[index]);
										   btn->setFlexGrow(1);
										   return btn;
									   });
	}
} // namespace UI
