/*
 * ExtruderControlPresenter.h
 *
 *  Created on: 2025-11-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class ExtruderControl;

	class ExtruderControlPresenter : public Presenter<ExtruderControl>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ExtruderControlPresenter, ExtruderControl);

		// Setters

		// Getters

		// Actions
		void extrude(float distance, float feedrate);

		// Observers
		void newToolData();
		void newStatus(OM::PrinterStatus status);

	  protected:
		void onInit() override;
		void onActivate() override;
		void onDeactivate() override {}

		void onConnect() override;
		void onDisconnect() override;

	  private:
		std::vector<std::string> m_filamentOptions;
	};
} // namespace UI
