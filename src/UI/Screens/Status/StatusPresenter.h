#pragma once

#include "UI/Core/Presenter.h"
#include "ObjectModel/PrinterStatus.h"

namespace UI
{
	class StatusView;

	class StatusPresenter : public Presenter<StatusView>
	{
	  public:
		using Presenter::Presenter;

		// Actions
		void pausePrint();
		void resumePrint();
		void printAgain();
		void cancelPrint();

		// Subscriptions
		virtual void newJobFileName(const char* filename) override;
		virtual void newJobLastFileName(const char* filename) override;
		virtual void newJobPrintTime() override;
		virtual void newJobDuration() override;
		virtual void newJobTimeLeft() override;
		virtual void newCurrentMoveRequestedSpeed() override;
		virtual void newCurrentMoveTopSpeed() override;
		virtual void newCurrentMoveExtrusionSpeed() override;
		virtual void newAxesData() override;
		virtual void newExtruderData() override;
		virtual void newSpeedFactor() override;
		virtual void newHeaterData() override;
		virtual void newFanData() override;
		virtual void newStatus(const OM::PrinterStatus status) override;

	  private:
		void onActivate() override;
		void onDeactivate() override;
		lv_timer_t* m_updateTimer;
	};
} // namespace UI
