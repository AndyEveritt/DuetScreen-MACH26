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

namespace UI
{
	class ToolListItem;
	class ToolList;

	class ToolListItemPresenter : public Presenter<ToolListItem>
	{
	  public:
		using Presenter::Presenter;

		void setSlotIndex(int8_t index);

		void newToolData() override;

		// Callbacks

		void setActiveTemp(int32_t value);

	  private:
		bool updateView(const OM::Tool* tool,
						const OM::ToolHeater* tHeater,
						const uint8_t tHeaterIndex,
						const OM::Spindle* spindle);
		bool updateView(const OM::BedOrChamber* bedOrChamber, const OM::Heat::Heater* heater, const bool bed);

		int8_t m_slotIndex = -1;
	};

	class ToolListPresenter : public Presenter<ToolList>
	{
	  public:
		using Presenter::Presenter;

		void newToolData() override;

		size_t getTotalHeaterCount(const bool addTools = true,
								   const bool addBeds = true,
								   const bool addChambers = true);

	  private:
	};
} // namespace UI
