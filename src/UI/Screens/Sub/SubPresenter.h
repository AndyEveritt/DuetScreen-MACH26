#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class SubView;

	class SubPresenter : public Presenter<SubView>
	{
	  public:
		using Presenter::Presenter;

	  protected:
	};
} // namespace UI
