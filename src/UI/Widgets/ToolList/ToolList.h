/*
 * ToolList.h
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#pragma once

#include "ToolListPresenter.h"
#include "UI/Components/Input/ModalNumberPad.h"
#include "UI/Components/List/List.h"
#include "UI/Core/View.h"
#include <memory>
#include <vector>

namespace UI
{
	class ToolListHeater : public ListItem
	{
	  public:
		using status_cb_t = std::function<void()>;
		using temperature_cb_t = std::function<void(bool isActive)>;

		ToolListHeater(size_t index, LvObj& parent);

		void setLabel(std::string_view text);
		void setStatus(std::string_view text);
		void setCurrentTemp(float value);
		void setActiveTemp(int32_t value);
		void setStandbyTemp(int32_t value);

		void setStatusCallback(status_cb_t cb) { m_statusCb = cb; }
		void setTemperatureCallback(temperature_cb_t cb) { m_temperatureCb = cb; }

	  private:
		static void onStatusEvent(lv_event_t* e);
		static void onActiveStandbyEvent(lv_event_t* e);

		LvLabel m_label{"label", getRoot()};
		Button m_status{"status", getRoot()};
		LvLabel m_currentTemp{"current", getRoot()};
		Button m_activeTemp{"active", getRoot()};
		Button m_standbyTemp{"standby", getRoot()};

		status_cb_t m_statusCb = nullptr;
		temperature_cb_t m_temperatureCb = nullptr;
	};

	class ToolListTool : public View<ToolListToolPresenter, ListItem>
	{
	  public:
		ToolListTool(size_t index, LvObj& parent, ToolList& toolList);

		void setLabel(std::string_view text);
		void setSelected(const bool selected);

		size_t setHeaterCount(size_t count);
		size_t getHeaterCount() const { return m_heaters.getItemCount(); }
		auto getHeater(size_t index) const { return m_heaters.getItem(index); }

	  private:
		static void onNameEvent(lv_event_t* e);

		Button m_toolName{"tool_name", getRoot()};
		List<ToolListHeater> m_heaters{"heaters", getRoot()};

		bool m_selected;
	};

	class ToolList : public View<ToolListPresenter>
	{
	  public:
		ToolList(const std::string& name, LvObj& parent, LvObj* numberPadParent = nullptr);

		size_t setToolCount(size_t count);
		size_t setBedCount(size_t count);
		size_t setChamberCount(size_t count);

		size_t getToolCount() const { return m_tools.getItemCount(); }
		size_t getBedCount() const { return m_beds.getItemCount(); }
		size_t getChamberCount() const { return m_chambers.getItemCount(); }

		auto getTool(size_t index) const { return m_tools.getItem(index); }
		auto getBed(size_t index) const { return m_beds.getItem(index); }
		auto getChamber(size_t index) const { return m_chambers.getItem(index); }

		auto& getNumberPad() { return m_numberPad; }
		void showNumberPad();
		void hideNumberPad() { m_numberPad.hide(); }

		void onHide() override { hideNumberPad(); }

	  private:
		void init();

		List<ToolListTool> m_tools{"tools", getRoot()};
		List<ToolListHeater> m_beds{"beds", getRoot()};
		List<ToolListHeater> m_chambers{"chambers", getRoot()};

		ModalNumberPad m_numberPad;
	};
} // namespace UI
