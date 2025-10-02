#pragma once

#include "ObjectModel/PrinterStatus.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class StatusView;

	class StatusPresenter : public Presenter<StatusView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(StatusPresenter, StatusView)

		// Actions
		void pausePrint();
		void resumePrint();
		void printAgain();
		void cancelPrint();

		// Subscriptions
		void newJobFileName(const std::string& filename);
		void newJobLastFileName(const std::string& filename);
		void newThumbnailData(const std::string& filename);
		void newJobPrintTime();
		void newJobDuration();
		void newJobTimeLeft();
		void newCurrentMoveRequestedSpeed();
		void newCurrentMoveTopSpeed();
		void newCurrentMoveExtrusionSpeed();
		void updateLayerInfo();
		void newAxesData();
		void newExtruderData();
		void newSpeedFactor();
		void newFanData();
		void newStatus(const OM::PrinterStatus status);

	  private:
		void onActivate() override;

		virtual void onInit() override
		{
			registerEventListener<EventType::JobFileName>(this, &StatusPresenter::newJobFileName);
			registerEventListener<EventType::JobLastFileName>(this, &StatusPresenter::newJobLastFileName);
			registerEventListener<EventType::JobPrintTime>(this, &StatusPresenter::newJobPrintTime);
			registerEventListener<EventType::ThumbnailData>(this, &StatusPresenter::newThumbnailData);
			registerEventListener<EventType::JobDuration>(this, &StatusPresenter::newJobDuration);
			registerEventListener<EventType::JobTimeLeft>(this, &StatusPresenter::newJobTimeLeft);
			registerEventListener<EventType::CurrentMoveRequestedSpeed>(this,
																		&StatusPresenter::newCurrentMoveRequestedSpeed);
			registerEventListener<EventType::CurrentMoveTopSpeed>(this, &StatusPresenter::newCurrentMoveTopSpeed);
			registerEventListener<EventType::CurrentMoveExtrusionSpeed>(this,
																		&StatusPresenter::newCurrentMoveExtrusionSpeed);
			registerEventListener<EventType::JobHeight>(this, &StatusPresenter::updateLayerInfo);
			registerEventListener<EventType::AxesData>(this, &StatusPresenter::newAxesData);
			registerEventListener<EventType::ExtruderData>(this, &StatusPresenter::newExtruderData);
			registerEventListener<EventType::SpeedFactor>(this, &StatusPresenter::newSpeedFactor);
			registerEventListener<EventType::FanData>(this, &StatusPresenter::newFanData);
			registerEventListener<EventType::Status>(this, &StatusPresenter::newStatus);
		}

		void setOrRequestThumbnail(const std::string& filename);

		lv_timer_t* m_updateTimer;
	};
} // namespace UI
