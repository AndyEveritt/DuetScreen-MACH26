/*
 * NumberPadPresenter.h
 *
 *  Created on: 2025-01-17
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"
#include "UI/Core/View.h"

namespace UI
{
	class NumberPad;

	class NumberPadPresenter : public Presenter<NumberPad>
	{
	  public:
		using Presenter::Presenter;
	};
} // namespace UI
