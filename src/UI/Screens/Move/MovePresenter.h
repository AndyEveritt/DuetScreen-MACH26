#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class MoveView;

	class MovePresenter : public Presenter<MoveView>
	{
	  public:
		using Presenter::Presenter;

		void homeAxis(size_t axisSlot);

		// oberservers

		virtual void newAxesData() override;

	  protected:
	};
} // namespace UI
