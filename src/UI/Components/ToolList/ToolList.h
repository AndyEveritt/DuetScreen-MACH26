/*
 * ToolList.h
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#pragma once

#include "ToolListPresenter.h"
#include "UI/Core/View.h"
#include <memory>
#include <vector>

namespace UI
{
	class ToolListItem : public View<ToolListItemPresenter>
	{
	  public:
		ToolListItem(const std::string& name, lv_obj_t* parent, layout_t layout);

		void setSlotIndex(uint8_t index);
		void setLabel(const char* text);
		void setIcon(lv_img_dsc_t* icon);
		void setStatus(const char* text);
		void setCurrentTemp(float value);
		void setActiveTemp(int32_t value);
		void setActiveTempText(const char* text);
		void setStandbyTemp(int32_t value);
		void setStandbyTempText(const char* text);
		void showTemps(bool show);

	  private:
		static void activeTempEvent(lv_event_t* e);

		lv_obj_t* m_label;
		lv_obj_t* m_icon;
		lv_obj_t* m_status;
		lv_obj_t* m_currentTemp;
		lv_obj_t* m_activeTemp;
		lv_obj_t* m_standbyTemp;
	};

	class ToolList : public View<ToolListPresenter>
	{
	  public:
		ToolList(const std::string& name, lv_obj_t* parent, layout_t layout);

		void setItemCnt(size_t cnt);
		size_t getItemCnt() const;
		std::shared_ptr<ToolListItem> getToolListItem(size_t index) const;

	  private:
		lv_obj_t* m_header;
		lv_obj_t* m_headerTool;
		lv_obj_t* m_headerStatus;
		lv_obj_t* m_headerCurrent;
		lv_obj_t* m_headerActive;
		lv_obj_t* m_headerStandby;
		lv_obj_t* m_list;
		std::vector<std::shared_ptr<ToolListItem>> m_items;
	};
} // namespace UI
