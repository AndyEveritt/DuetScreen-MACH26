#include "StatusView.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "lv_i18n/lv_i18n.h"

/**
 * @brief
 */
namespace UI
{
	/**
	 * @brief
	 * @param parent
	 */
	StatusView::StatusView(lv_obj_t* parent)
		: View(lv_obj_create, "print_view", parent, layout_t(0, 0, 100, 100))
		// Create all panels first
		, m_header(lv_obj_create(getCont()))
		, m_centerCont(lv_obj_create(getCont()))
		, m_footer(lv_obj_create(getCont()))
		// Create header widgets
		, m_progress(lv_arc_create(m_header))
		, m_progressPercent(lv_label_create(m_progress))
		, m_filename(lv_label_create(m_header))
		// Create all information widgets
		, m_thumbnail(lv_image_create(m_centerCont))
		, m_printInfo(m_centerCont)
		// Create control buttons last
		, m_pauseBtn("print_pause", m_footer, _("pause"))
		, m_resumeBtn("print_resume", m_footer, _("resume"))
		, m_printAgainBtn("print_again", m_footer, _("print_again"))
		, m_cancelBtn("print_cancel", m_footer, _("cancel"))
		, m_fineTuneBtn("fine_tune", m_footer, _("fine_tune"))
		, m_confirmCancel("print_confirm_cancel", getCont(), layout_t(0, 0, 70, LV_SIZE_CONTENT))
		, m_fineTune(getCont())
	{
		UI_LOCK();

		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);

		lv_obj_set_size(m_header, LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_width(m_centerCont, LV_PCT(100));
		lv_obj_set_flex_grow(m_centerCont, 1);
		lv_obj_set_size(m_footer, LV_PCT(100), LV_SIZE_CONTENT);

		// Header
		lv_obj_set_layout(m_header, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(m_header, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_size(m_progress, 100, 100);
		lv_obj_set_size(m_progressPercent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_height(m_filename, LV_SIZE_CONTENT);
		lv_obj_set_flex_grow(m_filename, 1);
		lv_arc_set_range(m_progress, 0, 100);
		lv_obj_remove_flag(m_progress, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_remove_style(m_progress, NULL, LV_PART_KNOB);
		lv_obj_center(m_progressPercent);

		// Central Container
		lv_obj_set_layout(m_centerCont, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(m_centerCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_centerCont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_size(m_thumbnail, LV_PCT(25), LV_PCT(100));
		lv_image_set_inner_align(m_thumbnail, LV_IMAGE_ALIGN_CONTAIN);
		lv_obj_set_flex_grow(m_printInfo, 1);
		lv_obj_set_height(m_printInfo, LV_PCT(100));

		// Print information
		lv_obj_set_style_pad_all(m_printInfo, 0, 0);

		// Footer
		lv_obj_set_layout(m_footer, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(m_footer, LV_FLEX_FLOW_ROW);

		for (size_t i = 0; i < lv_obj_get_child_cnt(m_footer); i++)
		{
			lv_obj_t* child = lv_obj_get_child(m_footer, i);
			lv_obj_set_height(child, LV_SIZE_CONTENT);
			lv_obj_set_flex_grow(child, 1);
		}

		// Hide resume button initially
		m_resumeBtn.hide();

		// Cancel confirmation setup
		m_confirmCancel.setMode(OM::Alert::Mode::ConfirmCancel);
		m_confirmCancel.setTitle(_("print_cancel_title"));
		m_confirmCancel.setText(_("print_cancel_message"));
		m_confirmCancel.hide();
		lv_obj_add_flag(m_confirmCancel.getCont(), LV_OBJ_FLAG_FLOATING);
		lv_obj_align(m_confirmCancel.getCont(), LV_ALIGN_CENTER, 0, 0);

		// Callbacks
		m_pauseBtn.setCallback(onPauseClicked, LV_EVENT_CLICKED, this);
		m_resumeBtn.setCallback(onResumeClicked, LV_EVENT_CLICKED, this);
		m_printAgainBtn.setCallback(onPrintAgainClicked, LV_EVENT_CLICKED, this);
		m_cancelBtn.setCallback(onCancelClicked, LV_EVENT_CLICKED, this);

		m_fineTuneBtn.setCallback(
			[](lv_event_t* e)
			{
				UI_LOCK();
				StatusView* view = static_cast<StatusView*>(lv_event_get_user_data(e));
				view->m_fineTune.show();
			},
			LV_EVENT_CLICKED,
			this);

		// Fine tune
		// Make the fine tune view floating and fullscreen
		lv_obj_add_flag(m_fineTune.getCont(), LV_OBJ_FLAG_FLOATING);
		lv_obj_set_size(m_fineTune.getCont(), LV_PCT(100), LV_PCT(100));
		lv_obj_align(m_fineTune.getCont(), LV_ALIGN_CENTER, 0, 0);
		m_fineTune.hide(); // Hide initially, will be shown when needed
	}

	bool StatusView::back()
	{
		UI_LOCK();
		if (m_fineTune.isVisible())
		{
			m_fineTune.hide();
			return true;
		}
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

	void StatusView::setFilename(const char* filename)
	{
		UI_LOCK();
		LOG_DBG("'{:s}'", filename);
		lv_label_set_text(m_filename, filename);
	}

	void StatusView::updateProgress(uint32_t percent)
	{
		UI_LOCK();
		LOG_DBG("{:d}", percent);
		percent = percent > 100 ? 100 : percent;

		lv_arc_set_value(m_progress, percent);
		lv_label_set_text(m_progressPercent, utils::format("%u%%", percent).c_str());
	}

	void StatusView::updateToolTemp(float temp, int32_t target)
	{
		m_printInfo.updateToolTemp(temp, target);
	}

	void StatusView::updateBedTemp(float temp, int32_t target)
	{
		m_printInfo.updateBedTemp(temp, target);
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
		lv_image_set_src(m_thumbnail, img);
	}

	void StatusView::setPause(ControlVisibility visibility)
	{
		UI_LOCK();

		m_pauseBtn.setInvalid(visibility != ENABLED);
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

		m_resumeBtn.setInvalid(visibility != ENABLED);
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

		m_printAgainBtn.setInvalid(visibility != ENABLED);
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

		m_cancelBtn.setInvalid(visibility != ENABLED);
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
