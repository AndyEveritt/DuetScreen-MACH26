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
#include "UI/Components/Input/NumberPad.h"
#include "UI/Core/Presenter.h"
#include "lvgl/lvgl.h"

namespace UI
{
	class ToolList;
	class ToolListTool;
	class ToolListToolPresenter;

	class ToolListPresenter : public Presenter<ToolList>
	{
		friend ToolListToolPresenter;

	  public:
		PRESENTER_CONSTRUCTOR(ToolListPresenter, ToolList)

		void update();

		size_t getTotalHeaterCount(const bool addTools = true,
								   const bool addBeds = true,
								   const bool addChambers = true);

	  private:
		void onInit() override
		{
			registerEventListener<EventType::ToolData>(this, &ToolListPresenter::update);
			registerEventListener<EventType::HeaterData>(this, &ToolListPresenter::update);
		}
		void onActivate() override;
		void onDeactivate() override;
		void onDisconnect() override { update(); };

		struct NumberPadConfig
		{
			std::string header;
			int32_t initialValue = 0;
			float min = 0.0f;
			float max = 0.0f;
			NumberPad::confirm_cb_t confirmCb = std::nullopt;
		};

		void configureNumberPad(const NumberPadConfig& config);
	};

	class ToolListToolPresenter : public Presenter<ToolListTool>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ToolListToolPresenter, ToolListTool)

		void setToolListPresenter(std::weak_ptr<ToolListPresenter> presenter) { m_toolListPresenter = presenter; }

		size_t getSlotIndex() const;

		void update();

		void toggleState();

	  private:
		void onInit() override
		{
			registerEventListener<EventType::ToolData>(this, &ToolListToolPresenter::update);
			registerEventListener<EventType::HeaterData>(this, &ToolListToolPresenter::update);
		}
		void onActivate() override;

		std::weak_ptr<ToolListPresenter> m_toolListPresenter;
		OM::ToolPtr m_tool;

		bool m_setActiveTemp = true;
	};
} // namespace UI
