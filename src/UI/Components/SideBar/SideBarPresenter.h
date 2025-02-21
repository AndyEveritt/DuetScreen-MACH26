/*
 * SideBarPresenter.h
 *
 *  Created on: 2025-02-21
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"
#include "Hardware/Duet.h"


namespace UI
{
	class SideBar;

	class SideBarPresenter : public Presenter<SideBar>
	{
	  public:
  		using Presenter::Presenter;

        void eStop();

	  protected:
	};
} // namespace UI
