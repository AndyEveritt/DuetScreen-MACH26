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
		void toggleToolState(size_t index);
		void updateFilamentList();
		void loadFilament(const std::string& filament);
		void unloadFilament();

		// Observers
        void newToolData();

	  protected:
		virtual void onInit() override;
		virtual void onActivate() override;
		virtual void onDeactivate() override {}

		virtual void onConnect() override;
		virtual void onDisconnect() override;

    private:
	  std::vector<std::string> m_filamentOptions;
	};
} // namespace UI
