/*
 * FilamentSelectPresenter.h
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class FilamentSelect;

	class FilamentSelectPresenter : public Presenter<FilamentSelect>
	{
	  public:
		PRESENTER_CONSTRUCTOR(FilamentSelectPresenter, FilamentSelect);

		// Setters

		// Getters

		// Actions

		// Observers

	  protected:
		virtual void onInit() override {}
		virtual void onActivate() override {}
		virtual void onDeactivate() override {}
		virtual void onDisconnect() {}
	};
} // namespace UI
