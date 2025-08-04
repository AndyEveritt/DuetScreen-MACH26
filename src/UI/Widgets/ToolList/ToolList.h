/*
 * ToolList.h
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#pragma once

#include "ToolListPresenter.h"
#include "UI/Components/Input/NumberPad.h"
#include "UI/Components/List/List.h"
#include "UI/Core/View.h"
#include <memory>
#include <vector>

namespace UI
{
	class ToolList;

	class ToolListItem : public View<ToolListItemPresenter>
	{
	  public:
		ToolListItem(size_t index, lv_obj_t* parent, ToolList& toolList);

		uint8_t getSlotIndex() const;
		void setSlotIndex(uint8_t index);
		void setLabel(std::string_view text);
		void setIcon(lv_img_dsc_t* icon);
		void setSelected(const bool selected);
		void setStatus(std::string_view text);
		void setCurrentTemp(float value);
		void setActiveTemp(int32_t value);
		void setActiveTempText(std::string_view text);
		void setStandbyTemp(int32_t value);
		void setStandbyTempText(std::string_view text);
		void showTemps(bool show);

		ToolList& getToolList() const { return m_toolList; }

	  private:
		static void onNameEvent(lv_event_t* e);
		static void onStatusEvent(lv_event_t* e);
		static void onActiveStandbyEvent(lv_event_t* e);

		ToolList& m_toolList;

		Button m_toolName;
		Button m_status;
		LvLabel m_currentTemp;
		LvLabel m_activeTemp;
		LvLabel m_standbyTemp;

		bool m_selected;
	};

	class ToolListNumPad : public LvObj
	{
	  public:
		using confirm_cb_t = NumberPad::confirm_cb_t;

		ToolListNumPad(const std::string& name, lv_obj_t* parent, layout_t layout);

		void clear() { m_numberPad.clear(); }
		void setMinValue(int16_t value) { m_numberPad.setMinValue(value); }
		void setMaxValue(int16_t value) { m_numberPad.setMaxValue(value); }
		void setValue(int16_t value) { m_numberPad.setValue(value); }
		int16_t getValue() const { return m_numberPad.getValue(); }
		void setHeader(const char* text) { lv_label_set_text(m_header, text); }
		const char* getHeader() const { return lv_label_get_text(m_header); }

		void setConfirmCallback(confirm_cb_t eventCb) { m_numberPad.setConfirmCallback(eventCb); }
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

		ToolList(const std::string& name, lv_obj_t* parent);
		ToolList(const std::string& name, lv_obj_t* parent, layout_t layout);

		void setItemCnt(size_t cnt);
		size_t getItemCnt() const { return m_list.getItemCount(); }
		std::shared_ptr<ToolListItem> getToolListItem(size_t index) const { return m_list.getItem(index); }

		void showNumberPad(const ToolListItem& item);
		void hideNumberPad() { m_numberPad.hide(); }

		virtual void onHide() override { hideNumberPad(); }

	  private:
		void init();

		LvContainer m_header;
		LvLabel m_headerTool;
		LvLabel m_headerStatus;
		LvLabel m_headerCurrent;
		LvLabel m_headerActive;
		LvLabel m_headerStandby;
		List<ToolListItem> m_list;

		ToolListNumPad m_numberPad;
	};
} // namespace UI
