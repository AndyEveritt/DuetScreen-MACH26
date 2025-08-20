#pragma once

#include "FineTune.h"
#include "PrintInfo.h"
#include "StatusPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Components/Modal/Modal.h"
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
		class Header : public LvContainer
		{
		  public:
			Header(const std::string& name, lv_obj_t* parent);
			void setProgress(uint32_t value);
			void setFilename(std::string_view name);
			void setThumbnail(const char* path);

		  private:
			LvBar m_progress;
			LvLabel m_progressLabel;
			LvLabel m_filename;
			LvImage m_thumbnail;
		};

		StatusView(lv_obj_t* parent);

		void setFilename(std::string_view filename);
		void updateProgress(uint32_t percent);

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
		Header m_headerPanel;
		PrintInfo m_printInfo;
		LvContainer m_footer;

		// Control buttons
		Button m_pauseBtn;
		Button m_resumeBtn;
		Button m_printAgainBtn;
		Button m_cancelBtn;
		Modal<MessageBox> m_confirmCancel;
	};
} // namespace UI
