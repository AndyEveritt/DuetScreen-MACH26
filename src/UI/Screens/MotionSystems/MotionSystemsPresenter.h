/*
 * MotionSystemsPresenter.h
 *
 *  Created on: 2026-03-24
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class MotionSystemsView;

	class MotionSystemsPresenter : public Presenter<MotionSystemsView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(MotionSystemsPresenter, MotionSystemsView)

		void newCurrentTool();
		void newSpeedFactor();
		void newCurrentMoveRequestedSpeed();
		void newCurrentMoveTopSpeed();
		void tick();

	  protected:
		void onInit() override;
		void onActivate() override;
		void onDisconnect() override;


	  private:
		void refreshTool();
		void refreshSpeedFactor();
		void refreshSpeeds();
	};
} // namespace UI