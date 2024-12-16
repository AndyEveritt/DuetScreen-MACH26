#pragma once

#include "UI/Core/presenter.h"

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
