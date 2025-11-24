/*
 * ExtrusionFactorPresenter.h
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#pragma once

#include "ObjectModel/Axis.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class ExtrusionFactor;
	class ModalExtrusionFactor;

	class ExtrusionFactorPresenter : public Presenter<ExtrusionFactor>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ExtrusionFactorPresenter, ExtrusionFactor);

		// Setters

		// Getters

		// Actions
		void setExtruderFactor(size_t slot, uint32_t value);

		// Observers
		void newExtruderData();

	  protected:
		void onInit() override
		{
			registerEventListener<EventType::ExtruderData>(this, &ExtrusionFactorPresenter::newExtruderData);
		}

		void onActivate() override { newExtruderData(); }

		void onDeactivate() override {}

		void onConnect() override {}
		void onDisconnect() override {}
	};

	class ModalExtrusionFactorPresenter : public Presenter<ModalExtrusionFactor>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ModalExtrusionFactorPresenter, ModalExtrusionFactor);

		// Setters

		// Getters

		// Actions
		void configureNumberPad(size_t index);

		// Observers

	  protected:
		void onInit() override {}
		void onActivate() override;
		void onDeactivate() override {}

		void onConnect() override {}
		void onDisconnect() override {}

		void configureNumberPad(OM::Move::ExtruderAxisPtr extruder);
	};
} // namespace UI
