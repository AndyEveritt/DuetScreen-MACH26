#include "StatusView.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"

/**
 * @brief
 */
namespace UI
{
	StatusView::StatusView(lv_obj_t* parent)
		: View("print_view", parent, layout_t(0, 0, 100, 100))
		// Create all panels first
		, m_headerPanel("header", getRoot())
		, m_printInfo(getRoot())
		, m_footer("footer", getRoot())

		// Create control buttons last
		, m_pauseBtn("print_pause", m_footer, _("pause"))
		, m_resumeBtn("print_resume", m_footer, _("resume"))
		, m_printAgainBtn("print_again", m_footer, _("print_again"))
		, m_cancelBtn("print_cancel", m_footer, _("cancel"))
		, m_confirmCancel("print_confirm_cancel", getRoot(), layout_t(0, 0, 70, LV_SIZE_CONTENT))
	{
		UI_LOCK();

		addStyle(Themes::getLvglStyles().bg_dark);
		m_headerPanel.addStyle(Themes::getLvglStyles().card);

		// Layout
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_headerPanel.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_printInfo.setWidth(LV_PCT(100));
		m_printInfo.setFlexGrow(1);
		m_footer.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		// Footer
		lv_obj_set_layout(m_footer, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(m_footer, LV_FLEX_FLOW_ROW);

		for (size_t i = 0; i < m_footer.getChildCnt(); i++)
		{
			lv_obj_t* child = m_footer.getChild(i);
			lv_obj_set_height(child, LV_SIZE_CONTENT);
			lv_obj_set_flex_grow(child, 1);
		}

		// Hide resume button initially
		m_resumeBtn.hide();

		// Cancel confirmation setup
		m_confirmCancel.setTitle(_("print_cancel_title"));
		m_confirmCancel.setText(_("print_cancel_message"));
		m_confirmCancel.okVisible(true);
		m_confirmCancel.cancelVisible(true);
		m_confirmCancel.hide();

		// Callbacks
		m_pauseBtn.addClickedCallback(onPauseClicked, this);
		m_resumeBtn.addClickedCallback(onResumeClicked, this);
		m_printAgainBtn.addClickedCallback(onPrintAgainClicked, this);
		m_cancelBtn.addClickedCallback(onCancelClicked, this);
	}

	StatusView::Header::Header(const std::string& name, lv_obj_t* parent)
		: LvContainer(name, parent)
		, m_progress("progress", getRoot())
		, m_progressLabel("progress_percent", m_progress)
		, m_filename("filename", getRoot())
		, m_thumbnail("thumbnail", getRoot())
	{
		UI_LOCK();

		static const int32_t col_dsc[] = {LV_GRID_FR(5), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		static const int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
		setGridDsc(col_dsc, row_dsc);
		setGridCell(m_filename, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
		setGridCell(m_thumbnail, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_progress, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 1, 1);

		// m_progress.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_progress.setRange(0, 100);
		// m_progressLabel.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_filename.setFlexGrow(1);
		// m_thumbnail.setSize(LV_PCT(20), LV_PCT(100));
	}

	void StatusView::Header::setFilename(std::string_view name)
	{
		m_filename.setText(name);
	}

	void StatusView::Header::setProgress(uint32_t percent)
	{
		m_progress.setValue(percent);
		m_progressLabel.setText(fmt::format("{:d}%", percent));
	}

	void StatusView::Header::setThumbnail(const char* img)
	{
		m_thumbnail.setSrc(img);
	}

	bool StatusView::back()
	{
		UI_LOCK();
		return m_printInfo.back();
	}

	void StatusView::onPauseClicked(lv_event_t* e)
	{
		UI_LOCK();
		StatusView* view = static_cast<StatusView*>(lv_event_get_user_data(e));
		view->m_presenter->pausePrint();
	}

	void StatusView::onResumeClicked(lv_event_t* e)
	{
		UI_LOCK();
		StatusView* view = static_cast<StatusView*>(lv_event_get_user_data(e));
		view->m_presenter->resumePrint();
	}

	void StatusView::onPrintAgainClicked(lv_event_t* e)
	{
		UI_LOCK();
		StatusView* view = static_cast<StatusView*>(lv_event_get_user_data(e));
		view->m_presenter->printAgain();
	}

	void StatusView::onCancelClicked(lv_event_t* e)
	{
		UI_LOCK();
		StatusView* view = static_cast<StatusView*>(lv_event_get_user_data(e));
		view->m_confirmCancel.setOkCallback([view]() { view->m_presenter->cancelPrint(); });
		view->m_confirmCancel.show();
	}

	void StatusView::onShow() {}

	void StatusView::onHide()
	{
		m_confirmCancel.hide();
	}

	void StatusView::setFilename(std::string_view filename)
	{
		UI_LOCK();
		LOG_DBG("'{:s}'", filename);
		m_headerPanel.setFilename(filename);
	}

	void StatusView::updateProgress(uint32_t percent)
	{
		UI_LOCK();
		LOG_DBG("{:d}", percent);
		percent = std::min(percent, 100u);
		m_headerPanel.setProgress(percent);
	}

	void StatusView::updateExtrusionRate(float feedrate, float volumetric)
	{
		m_printInfo.updateExtrusionRate(feedrate, volumetric);
	}

	void StatusView::updateSpeed(float topSpeed, float requestedSpeed)
	{
		m_printInfo.updateSpeed(topSpeed, requestedSpeed);
	}

	void StatusView::updateFlowMultiplier(uint32_t multiplier)
	{
		m_printInfo.updateFlowMultiplier(multiplier);
	}

	void StatusView::updateSpeedMultiplier(uint32_t multiplier)
	{
		m_printInfo.updateSpeedMultiplier(multiplier);
	}

	void StatusView::updateAcceleration(uint32_t acceleration)
	{
		m_printInfo.updateAcceleration(acceleration);
	}

	void StatusView::updatePosition(float x, float y, float z)
	{
		m_printInfo.updatePosition(x, y, z);
	}

	void StatusView::updateZOffset(float offset)
	{
		m_printInfo.updateZOffset(offset);
	}

	void StatusView::updateLayerNumber(uint32_t layer)
	{
		m_printInfo.updateLayerNumber(layer);
	}

	void StatusView::updateElapsedTime(uint32_t elapsed)
	{
		m_printInfo.updateElapsedTime(elapsed);
	}

	void StatusView::updateRemainingTime(uint32_t remaining)
	{
		m_printInfo.updateRemainingTime(remaining);
	}

	void StatusView::updateLayer(float height, float maxHeight)
	{
		m_printInfo.updateLayer(height, maxHeight);
	}

	void StatusView::updateFanSpeed(uint32_t speed)
	{
		m_printInfo.updateFanSpeed(speed);
	}

	void StatusView::setThumbnail(const char* img)
	{
		UI_LOCK();
		LOG_DBG("'{:s}'", img);
		m_headerPanel.setThumbnail(img);
	}

	void StatusView::setPause(ControlVisibility visibility)
	{
		UI_LOCK();

		m_pauseBtn.setDisabled(visibility != ENABLED);
		if (visibility == HIDDEN)
		{
			m_pauseBtn.hide();
		}
		else
		{
			m_pauseBtn.show();
		}
	}

	void StatusView::setResume(ControlVisibility visibility)
	{
		UI_LOCK();

		m_resumeBtn.setDisabled(visibility != ENABLED);
		if (visibility == HIDDEN)
		{
			m_resumeBtn.hide();
		}
		else
		{
			m_resumeBtn.show();
		}
	}

	void StatusView::setPrintAgain(ControlVisibility visibility)
	{
		UI_LOCK();

		m_printAgainBtn.setDisabled(visibility != ENABLED);
		if (visibility == HIDDEN)
		{
			m_printAgainBtn.hide();
		}
		else
		{
			m_printAgainBtn.show();
		}
	}

	void StatusView::setCancel(ControlVisibility visibility)
	{
		UI_LOCK();

		m_cancelBtn.setDisabled(visibility != ENABLED);
		if (visibility == HIDDEN)
		{
			m_cancelBtn.hide();
		}
		else
		{
			m_cancelBtn.show();
		}
	}
} // namespace UI
