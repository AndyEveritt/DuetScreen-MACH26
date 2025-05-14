#pragma once

#include "FineTune.h"
#include "PrintInfo.h"
#include "StatusPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Core/View.h"

namespace UI
{
	class StatusView : public View<StatusPresenter>
	{
	  public:
		enum ControlVisibility
		{
			ENABLED,
			DISABLED,
			HIDDEN
		};

		StatusView(lv_obj_t* parent);

		void setFilename(const char* filename);
		void updateProgress(uint32_t percent);

		void updateToolTemp(float temp, int32_t target);
		void updateBedTemp(float temp, int32_t target);
		void updateExtrusionRate(float feedrate, float volumetric);
		void updateSpeed(float topSpeed, float requestedSpeed);
		void updateFlowMultiplier(uint32_t multiplier);
		void updateSpeedMultiplier(uint32_t multiplier);
		void updateAcceleration(uint32_t acceleration);
		void updatePosition(float x, float y, float z);
		void updateZOffset(float offset);
		void updateLayerNumber(uint32_t layer);
		void updateElapsedTime(uint32_t elapsed);
		void updateRemainingTime(uint32_t remaining);
		void updateLayer(float height, float maxHeight);
		void updateFanSpeed(uint32_t speed);
		void setThumbnail(const char* img);

		void setPause(ControlVisibility visibility);
		void setResume(ControlVisibility visibility);
		void setPrintAgain(ControlVisibility visibility);
		void setCancel(ControlVisibility visibility);

		virtual bool back() override;

	  private:
		void onShow() override;
		void onHide() override;

		static void onPauseClicked(lv_event_t* e);
		static void onResumeClicked(lv_event_t* e);
		static void onPrintAgainClicked(lv_event_t* e);
		static void onCancelClicked(lv_event_t* e);

		// Status panels
		lv_obj_t* m_header;
		lv_obj_t* m_centerCont;
		lv_obj_t* m_footer;

		// Header
		lv_obj_t* m_progress;
		lv_obj_t* m_progressPercent;
		lv_obj_t* m_filename;

		// Print information widgets
		lv_obj_t* m_thumbnail;
		PrintInfo m_printInfo;

		// Control buttons
		Button m_pauseBtn;
		Button m_resumeBtn;
		Button m_printAgainBtn;
		Button m_cancelBtn;
		Button m_fineTuneBtn;
		MessageBox m_confirmCancel;

		// Fine tune
		FineTune m_fineTune;
	};
} // namespace UI
