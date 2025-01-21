#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class SettingsView;

	class SettingsPresenter : public Presenter<SettingsView>
	{
	  public:
		using Presenter::Presenter;

	  protected:
	};
} // namespace UI
