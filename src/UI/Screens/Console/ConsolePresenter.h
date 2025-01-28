#pragma once

#include "Duet3D/General/CircularBuffer.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class ConsoleView;

	class ConsolePresenter : public Presenter<ConsoleView>
	{
	  public:
		using Presenter::Presenter;

		// Actions
		bool back();

		// Observers
		void newResponse(const char* resp) override;

	  private:
		void onActivate() override;

		CircularBuffer<std::string, MAX_RESPONSE_LINES> m_buffer;
	};
} // namespace UI
