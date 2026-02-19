#pragma once

#include "FineTune.h"
#include "StatusPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Components/Modal/Modal.h"
#include "UI/Core/View.h"
#include "UI/Widgets/Status/PrintInfo/PrintInfo.h"

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

		StatusView(const std::string& name, LvObj& parent);

		void setFilename(std::string_view filename, bool isInProgress);
		void updateProgress(uint32_t percent);

		void setThumbnail(const char* img);

		void setPause(ControlVisibility visibility);
		void setResume(ControlVisibility visibility);
		void setPrintAgain(ControlVisibility visibility);
		void setCancel(ControlVisibility visibility);

		void setNumberPad(ModalNumberPad* np);

		bool back() override;

	  private:
		void onInit() override;
		void onShow() override;
		void onHide() override;

		static void onPauseClicked(lv_event_t* e);
		static void onResumeClicked(lv_event_t* e);
		static void onPrintAgainClicked(lv_event_t* e);
		static void onCancelClicked(lv_event_t* e);

		// Status panels
		LvContainer m_header{"header", getRoot()};
		PrintInfo m_printInfo{"print_info", getRoot()};
		LvContainer m_footer{"footer", getRoot()};

		// Header
		Bar m_progress{"progress", m_header};
		LvLabel m_printFinishedLabel{"printed_tag", m_header};
		LvLabel m_filename{"filename", m_header};
		LvImage m_thumbnail{"thumbnail", m_header};

		// Control buttons
		Button m_pauseBtn{"pause", m_footer};
		Button m_resumeBtn{"resume", m_footer};
		Button m_printAgainBtn{"print_again", m_footer};
		Button m_cancelBtn{"cancel", m_footer};

		Modal<MessageBox> m_confirmCancel{"start_print", getRoot(), layout_t(0, 0, 70, LV_SIZE_CONTENT)};
	};
} // namespace UI
