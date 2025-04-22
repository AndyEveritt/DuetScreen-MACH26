/*
 * ToolListPresenter.h
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#pragma once

#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Heat.h"
#include "ObjectModel/Spindle.h"
#include "ObjectModel/Tool.h"
#include "UI/Core/Presenter.h"
#include "lvgl/lvgl.h"

namespace UI
{
	class ToolListItem;
	class ToolList;

	class ToolListItemPresenter : public Presenter<ToolListItem>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ToolListItemPresenter, ToolListItem)

		int8_t getSlotIndex() const;
		void setSlotIndex(int8_t index);

		void newToolData() override;
		void newHeaterData() override;

		void update();

		void setTemp(int32_t value);
		bool configureNumberPad(const bool active);
		void toggleState();
		void toggleSubState();

	  private:
		static void numberPadConfirmCallback(lv_event_t* e);

		bool updateView(const std::shared_ptr<OM::Tool> tool,
						const std::shared_ptr<OM::ToolHeater> tHeater,
						const uint8_t tHeaterIndex,
						const std::shared_ptr<OM::Spindle> spindle);
		bool updateView(const std::shared_ptr<OM::BedOrChamber> bedOrChamber,
						const std::shared_ptr<OM::Heat::Heater> heater,
						const bool bed);

		int8_t m_slotIndex = -1;

		enum class SlotType
		{
			Tool,
			Bed,
			Chamber,
			Unknown
		} m_slotType = SlotType::Unknown;

		std::shared_ptr<OM::Tool> m_tool;
		std::shared_ptr<OM::ToolHeater> m_tHeater;
		std::shared_ptr<OM::Spindle> m_spindle;
		std::shared_ptr<OM::BedOrChamber> m_bedOrChamber;

		bool m_setActiveTemp = true;
	};

	class ToolListPresenter : public Presenter<ToolList>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ToolListPresenter, ToolList)

		void newToolData() override;
		void newHeaterData() override;

		void update();

		size_t getTotalHeaterCount(const bool addTools = true,
								   const bool addBeds = true,
								   const bool addChambers = true);

	  private:
	};
} // namespace UI
