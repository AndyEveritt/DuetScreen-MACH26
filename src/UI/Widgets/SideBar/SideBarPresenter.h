/*
 * SideBarPresenter.h
 *
 *  Created on: 2025-02-21
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class SideBar;

	class SideBarPresenter : public Presenter<SideBar>
	{
	  public:
		PRESENTER_CONSTRUCTOR(SideBarPresenter, SideBar)

		void eStop();

	  protected:
		void enableBackButton(bool enable);
		void enableHomeButton(bool enable);

		void onInit() override;
		void onActivate() override;
	};
} // namespace UI
