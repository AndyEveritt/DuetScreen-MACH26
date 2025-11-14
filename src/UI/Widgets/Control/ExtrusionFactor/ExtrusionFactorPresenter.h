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
		virtual void onInit() override
		{
			registerEventListener<EventType::ExtruderData>(this, &ExtrusionFactorPresenter::newExtruderData);
		}

		virtual void onActivate() override { newExtruderData(); }

		virtual void onDeactivate() override {}

		virtual void onConnect() override {}
		virtual void onDisconnect() override {}
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
		virtual void onInit() override {}
		virtual void onActivate() override;
		virtual void onDeactivate() override {}

		virtual void onConnect() override {}
		virtual void onDisconnect() override {}

		void configureNumberPad(OM::Move::ExtruderAxisPtr extruder);
	};
} // namespace UI
