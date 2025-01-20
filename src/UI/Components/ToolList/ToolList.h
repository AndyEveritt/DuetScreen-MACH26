/*
 * ToolList.h
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#pragma once

#include "ToolListPresenter.h"
#include "UI/Components/NumberPad/NumberPad.h"
#include "UI/Core/View.h"
#include <memory>
#include <vector>

namespace UI
{
	class ToolList;

	class ToolListItem : public View<ToolListItemPresenter>
	{
	  public:
		ToolListItem(ToolList& toolList, const std::string& name, lv_obj_t* parent, layout_t layout);

		uint8_t getSlotIndex() const;
		void setSlotIndex(uint8_t index);
		void setLabel(const char* text);
		void setIcon(lv_img_dsc_t* icon);
		void setSelected(const bool selected);
		void setStatus(const char* text);
		void setCurrentTemp(float value);
		void setActiveTemp(int32_t value);
		void setActiveTempText(const char* text);
		void setStandbyTemp(int32_t value);
		void setStandbyTempText(const char* text);
		void showTemps(bool show);

		ToolList& getToolList() const { return m_toolList; }

	  private:
		static void onNameEvent(lv_event_t* e);
		static void onStatusEvent(lv_event_t* e);
		static void onActiveStandbyEvent(lv_event_t* e);

		ToolList& m_toolList;

		lv_obj_t* m_label;
		lv_obj_t* m_icon;
		lv_obj_t* m_status;
		lv_obj_t* m_currentTemp;
		lv_obj_t* m_activeTemp;
		lv_obj_t* m_standbyTemp;
		bool m_selected;
	};

	class ToolListNumPad : public BaseView
	{
	  public:
		ToolListNumPad(const std::string& name, lv_obj_t* parent, layout_t layout);

		void clear() { m_numberPad.clear(); }
		void setMinValue(int16_t value) { m_numberPad.setMinValue(value); }
		void setMaxValue(int16_t value) { m_numberPad.setMaxValue(value); }
		void setValue(int16_t value) { m_numberPad.setValue(value); }
		int16_t getValue() const { return m_numberPad.getValue(); }
		void setHeader(const char* text) { lv_label_set_text(m_header, text); }
		const char* getHeader() const { return lv_label_get_text(m_header); }

		void setConfirmCallback(lv_event_cb_t eventCb, void* userData)
		{
			m_numberPad.setConfirmCallback(eventCb, userData);
		}
		virtual bool back() override;

	  private:
		lv_obj_t* m_header;
		NumberPad m_numberPad;
	};

	class ToolList : public View<ToolListPresenter>
	{
	  public:
		friend class ToolListPresenter;
		friend class ToolListItemPresenter;

		ToolList(const std::string& name, lv_obj_t* parent, layout_t layout);

		void setItemCnt(size_t cnt);
		size_t getItemCnt() const;
		std::shared_ptr<ToolListItem> getToolListItem(size_t index) const;

		void showNumberPad(const ToolListItem& item);
		void hideNumberPad() { m_numberPad.hide(); }

		virtual void onHide() override { hideNumberPad(); }

	  private:
		lv_obj_t* m_header;
		lv_obj_t* m_headerTool;
		lv_obj_t* m_headerStatus;
		lv_obj_t* m_headerCurrent;
		lv_obj_t* m_headerActive;
		lv_obj_t* m_headerStandby;
		lv_obj_t* m_list;
		std::vector<std::shared_ptr<ToolListItem>> m_items;

		ToolListNumPad m_numberPad;
	};
} // namespace UI
