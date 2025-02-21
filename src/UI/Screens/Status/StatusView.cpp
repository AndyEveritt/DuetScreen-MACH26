#include "StatusView.h"
#include "Debug.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	StatusView::StatusView(lv_obj_t* parent)
		: View("print_view", parent, layout_t(0, 0, 100, 100))
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
		, m_printInfoCont(lv_obj_create(m_centerCont))
		, m_toolTemp(lv_label_create(m_printInfoCont))
		, m_bedTemp(lv_label_create(m_printInfoCont))
		, m_speed(lv_label_create(m_printInfoCont))
		, m_speedMultiplier(lv_label_create(m_printInfoCont))
		, m_flowRate(lv_label_create(m_printInfoCont))
		, m_flowMultiplier(lv_label_create(m_printInfoCont))
		, m_elapsedTime(lv_label_create(m_printInfoCont))
		, m_remainingTime(lv_label_create(m_printInfoCont))
		, m_layer(lv_label_create(m_printInfoCont))
		, m_fanSpeed(lv_label_create(m_printInfoCont))
		// Create control buttons last
		, m_pauseBtn("print_pause", m_footer, _("pause"))
		, m_resumeBtn("print_resume", m_footer, _("resume"))
		, m_printAgainBtn("print_again", m_footer, _("print_again"))
		, m_cancelBtn("print_cancel", m_footer, _("cancel"))
		, m_confirmCancel("print_confirm_cancel", getCont(), layout_t(0, 0, 70, LV_SIZE_CONTENT))
	{
		Lock lock;

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
		lv_obj_set_flex_grow(m_printInfoCont, 1);
		lv_obj_set_height(m_printInfoCont, LV_PCT(100));

		// Print Info Container
		lv_obj_set_layout(m_printInfoCont, LV_LAYOUT_GRID);
		lv_obj_set_grid_align(m_printInfoCont, LV_GRID_ALIGN_SPACE_AROUND, LV_GRID_ALIGN_SPACE_AROUND);
		static int32_t printInfoColDsc[] = {LV_GRID_FR(2), LV_GRID_FR(3), LV_GRID_FR(5), LV_GRID_TEMPLATE_LAST};
		static int32_t printInfoRowDsc[] = {
			LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		lv_obj_set_grid_dsc_array(m_printInfoCont, printInfoColDsc, printInfoRowDsc);
		lv_obj_set_grid_cell(m_toolTemp, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
		lv_obj_set_grid_cell(m_bedTemp, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
		lv_obj_set_grid_cell(m_speedMultiplier, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_speed, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_flowMultiplier, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
		lv_obj_set_grid_cell(m_flowRate, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
		lv_obj_set_grid_cell(m_elapsedTime, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 3, 1);
		lv_obj_set_grid_cell(m_remainingTime, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 4, 1);
		lv_obj_set_grid_cell(m_layer, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_fanSpeed, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);

		for (size_t i = 0; i < lv_obj_get_child_cnt(m_printInfoCont); i++)
		{
			lv_obj_t* child = lv_obj_get_child(m_printInfoCont, i);
			lv_obj_set_align(child, LV_ALIGN_LEFT_MID);
			lv_obj_set_width(child, LV_PCT(100));
			lv_obj_set_style_min_height(child, 20, 0);
			// lv_obj_set_style_text_align(child, LV_TEXT_ALIGN_LEFT, 0);
			// lv_obj_set_height(child, LV_SIZE_CONTENT);
		}

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
	}

	void StatusView::onPauseClicked(lv_event_t* e)
	{
		Lock lock;
		StatusView* view = static_cast<StatusView*>(lv_event_get_user_data(e));
		view->m_presenter.pausePrint();
	}

	void StatusView::onResumeClicked(lv_event_t* e)
	{
		Lock lock;
		StatusView* view = static_cast<StatusView*>(lv_event_get_user_data(e));
		view->m_presenter.resumePrint();
	}

	void StatusView::onPrintAgainClicked(lv_event_t* e)
	{
		Lock lock;
		StatusView* view = static_cast<StatusView*>(lv_event_get_user_data(e));
		view->m_presenter.printAgain();
	}

	void StatusView::onCancelClicked(lv_event_t* e)
	{
		Lock lock;
		StatusView* view = static_cast<StatusView*>(lv_event_get_user_data(e));
		view->m_confirmCancel.setOkCallback([view]() { view->m_presenter.cancelPrint(); });
		view->m_confirmCancel.show();
	}

	void StatusView::onShow() {}

	void StatusView::onHide()
	{
		m_confirmCancel.hide();
	}

	void StatusView::setFilename(const char* filename)
	{
		Lock lock;
		lv_label_set_text(m_filename, filename);
	}

	void StatusView::updateProgress(uint32_t percent)
	{
		Lock lock;

		percent = percent > 100 ? 100 : percent;

		lv_arc_set_value(m_progress, percent);
		lv_label_set_text(m_progressPercent, utils::format("%u%%", percent).c_str());
	}

	void StatusView::updateToolTemp(float temp, int32_t target)
	{
		Lock lock;
		lv_label_set_text(m_toolTemp, utils::format(_("status_tool_temp"), temp, target).c_str());
	}

	void StatusView::updateBedTemp(float temp, int32_t target)
	{
		Lock lock;
		lv_label_set_text(m_bedTemp, utils::format(_("status_bed_temp"), temp, target).c_str());
	}

	void StatusView::updateExtrusionRate(float feedrate, float volumetric)
	{
		Lock lock;
		lv_label_set_text(m_speed, utils::format(_("status_speed"), feedrate).c_str());
		lv_label_set_text(m_flowRate, utils::format(_("status_flow_rate"), volumetric).c_str());
	}

	void StatusView::updateSpeed(float topSpeed, float requestedSpeed)
	{
		Lock lock;
		lv_label_set_text(m_speed, utils::format(_("status_speed"), topSpeed, requestedSpeed).c_str());
	}

	void StatusView::updateFlowMultiplier(uint32_t multiplier)
	{
		Lock lock;
		lv_label_set_text(m_flowMultiplier, utils::format(_("status_flow_multiplier"), multiplier).c_str());
	}

	void StatusView::updateSpeedMultiplier(uint32_t multiplier)
	{
		Lock lock;
		lv_label_set_text(m_speedMultiplier, utils::format(_("status_speed_multiplier"), multiplier).c_str());
	}

	void StatusView::updateElapsedTime(uint32_t elapsed)
	{
		Lock lock;
		int32_t hours = elapsed / 3600;
		int32_t minutes = (elapsed % 3600) / 60;
		int32_t seconds = elapsed % 60;
		std::string elapsedStr = utils::format("%02d:%02d:%02d", hours, minutes, seconds);
		lv_label_set_text(m_elapsedTime, utils::format(_("status_elapsed_time"), elapsedStr.c_str()).c_str());
	}

	void StatusView::updateRemainingTime(uint32_t remaining)
	{
		Lock lock;
		int32_t hours = remaining / 3600;
		int32_t minutes = (remaining % 3600) / 60;
		int32_t seconds = remaining % 60;
		std::string remainingStr = utils::format("%02d:%02d:%02d", hours, minutes, seconds);
		lv_label_set_text(m_remainingTime, utils::format(_("status_remaining_time"), remainingStr.c_str()).c_str());
	}

	void StatusView::updateLayer(float height, float maxHeight)
	{
		Lock lock;
		lv_label_set_text(m_layer, utils::format(_("status_layer"), height, maxHeight).c_str());
	}

	void StatusView::updateFanSpeed(uint32_t speed)
	{
		Lock lock;
		lv_label_set_text(m_fanSpeed, utils::format(_("status_fan_speed"), speed).c_str());
	}

	void StatusView::setThumbnail(lv_img_dsc_t* img)
	{
		Lock lock;
		lv_image_set_src(m_thumbnail, img);
	}

	void StatusView::setPause(ControlVisibility visibility)
	{
		Lock lock;

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
		Lock lock;

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
		Lock lock;

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
		Lock lock;

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
