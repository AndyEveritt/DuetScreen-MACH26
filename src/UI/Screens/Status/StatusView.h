#pragma once

#include "StatusPresenter.h"
#include "UI/Components/Button.h"
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
		void updateElapsedTime(uint32_t elapsed);
		void updateRemainingTime(uint32_t remaining);
		void updateLayer(float height, float maxHeight);
		void updateFanSpeed(uint32_t speed);
		void setThumbnail(lv_img_dsc_t* img);

		void setPause(ControlVisibility visibility);
		void setResume(ControlVisibility visibility);
		void setPrintAgain(ControlVisibility visibility);
		void setCancel(ControlVisibility visibility);

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
		lv_obj_t* m_printInfoCont;
		lv_obj_t* m_toolTemp;
		lv_obj_t* m_bedTemp;
		lv_obj_t* m_speed;
		lv_obj_t* m_speedMultiplier;
		lv_obj_t* m_flowRate;
		lv_obj_t* m_flowMultiplier;
		lv_obj_t* m_elapsedTime;
		lv_obj_t* m_remainingTime;
		lv_obj_t* m_layer;
		lv_obj_t* m_fanSpeed;

		// Control buttons
		Button m_pauseBtn;
		Button m_resumeBtn;
		Button m_printAgainBtn;
		Button m_cancelBtn;
		MessageBox m_confirmCancel;
	};
} // namespace UI
