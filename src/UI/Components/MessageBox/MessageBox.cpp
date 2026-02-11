/*
 * MessageBox.cpp
 *
 *  Created on: 2025-01-29
 *      Author: Andy Everitt
 */

#include "MessageBox.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"
#include "lvgl/src/lvgl_private.h"
#include "nameof.hpp"
#include "utils/StorageHelper.h"

namespace UI
{

	MessageBox::MessageBox(const std::string& name, LvObj& parent, layout_t layout)
		: LvContainer(name, parent, layout)
	{
		ZoneScoped;
		UI_LOCK();
		init();
	}

	MessageBox::~MessageBox()
	{
		ZoneScoped;
		UI_LOCK();
		if (!lv_is_initialized())
		{
			return;
		}
		if (m_timers.timeout != nullptr)
		{
			lv_timer_delete(m_timers.timeout);
		}
		if (m_timers.progress != nullptr)
		{
			lv_timer_delete(m_timers.progress);
		}
	}

	void MessageBox::onHide()
	{
		ZoneScoped;
		close();
	}

	void MessageBox::init()
	{
		ZoneScoped;
		UI_LOCK();

		addStyle(Themes::getLvglStyles().card);
		m_header.addStyle(Themes::getLvglStyles().bg_color_header);
		m_header.addStyle(Themes::getLvglStyles().text_header);

		// Layout
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_header.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_body.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_footer.setFlexFlow(LV_FLEX_FLOW_ROW);

		m_header.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_body.setWidth(LV_PCT(100));
		m_body.setHeight(LV_SIZE_CONTENT);
		m_pad.setWidth(LV_PCT(100));
		m_pad.setFlexGrow(1);
		m_footer.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		// Header

		// Body
		m_bodyTop.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_bodyTop.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_bodyTop.setStylePad(0);
		m_bodyTextCont.setFlexGrow(1);
		m_bodyTextCont.setHeight(LV_SIZE_CONTENT);
		m_bodyTextCont.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_bodyTextCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_bodyTextCont.setStyleTextAlign(LV_TEXT_ALIGN_CENTER, 0);
		m_text.setWidth(LV_PCT(100));
		m_image.setMinWidth(50, LV_PART_MAIN);
		m_image.setMaxWidth(300, LV_PART_MAIN);
		m_image.setMinHeight(50, LV_PART_MAIN);
		m_image.setMaxHeight(300, LV_PART_MAIN);
		m_image.setInnerAlign(LV_IMAGE_ALIGN_CONTAIN);

		// Bottom Container
		m_cancelBtn.setHeight(LV_SIZE_CONTENT);
		m_cancelBtn.setFlexGrow(1);
		m_okBtn.setHeight(LV_SIZE_CONTENT);
		m_okBtn.setFlexGrow(1);
		m_cancelBtn.setText(_("common.cancel"));
		m_okBtn.setText(_("common.ok"));

		m_progress.setSize(LV_PCT(100), 5);

		// Callbacks
		m_okBtn.addClickedCallback(onOkEvent, this);
		m_cancelBtn.addClickedCallback(onCancelEvent, this);

		m_timers.progress = lv_timer_create(onProgressTimer, 50, this);
		lv_timer_set_auto_delete(m_timers.progress, false);

		// addEventCallback(
		// 	[](lv_event_t* e)
		// 	{
		// 		auto& msgBox = *static_cast<MessageBox*>(lv_event_get_user_data(e));
		// 		lv_coord_t height = msgBox.getHeight();
		// 		lv_coord_t header_height = msgBox.m_header.getHeight();
		// 		lv_coord_t footer_height = msgBox.m_footer.getHeight();

		// 		msgBox.m_body.setHeight(height - header_height - footer_height);
		// 		return;
		// 	},
		// 	LV_EVENT_SIZE_CHANGED,
		// 	this);

		clear();
	}

	void MessageBox::ok()
	{
		ZoneScoped;
		UI_LOCK();
		auto self = getPtr(); // Keep shared ptr to self to prevent deletion during callback
		if (m_okCb)
		{
			LOG_INFO("Calling ok callback");
			m_okCb();
		}
		close();
	}

	void MessageBox::cancel()
	{
		ZoneScoped;
		UI_LOCK();
		auto self = getPtr(); // Keep shared ptr to self to prevent deletion during callback
		if (m_cancelCb)
		{
			LOG_INFO("Calling cancel callback");
			m_cancelCb();
		}
		close();
	}

	void MessageBox::close()
	{
		ZoneScoped;
		UI_LOCK();
		if (getRootPtr()) // This stops an infrequent segfault when HomePresenter destroys the response message boxes
		{
			hide();
		}
		if (m_closeCb)
		{
			LOG_INFO("Calling close callback");
			m_closeCb();
		}
	}

	void MessageBox::setTitle(std::string_view text)
	{
		ZoneScoped;
		UI_LOCK();
		m_title.setText(text);
	}

	void MessageBox::setText(std::string_view text)
	{
		ZoneScoped;
		UI_LOCK();
		m_text.setText(text);
		m_text.setVisible(!text.empty());
	}

	void MessageBox::setImage(const char* imagePath)
	{
		ZoneScoped;
		UI_LOCK();
		m_image.setSrc(imagePath);
		if (m_autoSizeImage)
		{
			m_image.setSize(m_image.getSrcWidth(), m_image.getSrcHeight());
		}
		imageVisible(imagePath != nullptr);
	}

	void MessageBox::setImageSize(int32_t width, int32_t height)
	{
		ZoneScoped;
		UI_LOCK();
		autoSizeImage(false);
		m_image.setSize(width, height);
	}

	void MessageBox::setOkBtnText(std::string_view text)
	{
		ZoneScoped;
		UI_LOCK();
		m_okBtn.setText(text);
	}

	void MessageBox::setCancelBtnText(std::string_view text)
	{
		ZoneScoped;
		UI_LOCK();
		m_cancelBtn.setText(text);
	}

	void MessageBox::clear()
	{
		ZoneScoped;
		UI_LOCK();
		setTitle("");
		setText("");
		setImage(nullptr);
		setOkBtnText(_("common.ok"));
		setCancelBtnText(_("common.cancel"));
		okVisible(true);
		cancelVisible(false);
		imageVisible(false);
		progressVisible(false);

		setCancelCallback(nullptr);
		setOkCallback(nullptr);
		setCloseCallback(nullptr);
		setProgressCallback(nullptr);
	}

	void MessageBox::okVisible(bool visible)
	{
		ZoneScoped;
		UI_LOCK();
		m_okBtn.setVisible(visible);
		updateVisibility();
	}

	void MessageBox::cancelVisible(bool visible)
	{
		ZoneScoped;
		UI_LOCK();
		m_cancelBtn.setVisible(visible);
		updateVisibility();
	}

	void MessageBox::imageVisible(bool visible)
	{
		ZoneScoped;
		UI_LOCK();
		m_image.setVisible(visible);
		m_bodyTextCont.setStyleTextAlign(visible ? LV_TEXT_ALIGN_LEFT : LV_TEXT_ALIGN_CENTER, 0);
		updateVisibility();
	}

	void MessageBox::progressVisible(bool visible)
	{
		ZoneScoped;
		UI_LOCK();
		m_progress.setVisible(visible);
	}

	void MessageBox::updateVisibility()
	{
		ZoneScoped;
		UI_LOCK();
		bool visible = false;
		m_body.iterateChildrenWhile(
			[&visible](size_t /* index */, LvObj& child)
			{
				if (!child.hasFlag(LV_OBJ_FLAG_HIDDEN))
				{
					visible = true;
					return false;
				}
				return true;
			});
		m_body.setVisible(visible);

		visible = false;
		m_footer.iterateChildrenWhile(
			[&visible](size_t /* index */, LvObj& child)
			{
				if (!child.hasFlag(LV_OBJ_FLAG_HIDDEN))
				{
					visible = true;
					return false;
				}
				return true;
			});
		m_footer.setVisible(visible);
	}

	void MessageBox::setProgress(int percent)
	{
		ZoneScoped;
		UI_LOCK();
		m_progress.setValue(percent, LV_ANIM_ON);
	}

	void MessageBox::cancelTimeout()
	{
		ZoneScoped;
		UI_LOCK();
		if (m_timers.timeout)
		{
			lv_timer_delete(m_timers.timeout);
			m_timers.timeout = nullptr;
		}
		m_timeout = 0;
	}

	void MessageBox::setTimeout(std::chrono::milliseconds timeout)
	{
		ZoneScoped;
		UI_LOCK();
		if (m_type == ResponseType::ERROR && !StorageHelper::getData(ID_NOTIFICATION_AUTO_CLOSE_ERROR))
		{
			return;
		}

		m_timeout = static_cast<uint32_t>(timeout.count());
		if (m_timeout == 0)
		{
			if (m_timers.timeout)
			{
				lv_timer_delete(m_timers.timeout);
				m_timers.timeout = nullptr;
			}
			return;
		}
		m_timers.timeout = lv_timer_create(
			[](lv_timer_t* timer)
			{
				UI_LOCK();
				MessageBox* msgBox = static_cast<MessageBox*>(lv_timer_get_user_data(timer));
				if (msgBox->isValid())
				{
					msgBox->cancel();
				}
			},
			m_timeout,
			this);
		lv_timer_set_auto_delete(m_timers.timeout, false);
		lv_timer_set_repeat_count(m_timers.timeout, 1);
	}

	uint32_t MessageBox::getTimeRemaining() const
	{
		ZoneScoped;
		UI_LOCK();
		if (m_timers.timeout)
		{
			uint32_t elaps = lv_tick_elaps(m_timers.timeout->last_run);
			if (elaps < m_timeout)
			{
				return m_timeout - elaps;
			}
		}
		return 0;
	}

	uint32_t MessageBox::getTimeOutPercentage() const
	{
		ZoneScoped;
		UI_LOCK();
		if (m_timeout == 0)
		{
			return 0u;
		}
		LOG_VERBOSE("Time remaining: {:d}", getTimeRemaining());
		return 100 * getTimeRemaining() / m_timeout;
	}

	void MessageBox::setType(ResponseType type)
	{
		ZoneScoped;
		UI_LOCK();
		m_type = type;

		removeStyle(Themes::getLvglStyles().bg_color_success);
		removeStyle(Themes::getLvglStyles().bg_color_warning);
		removeStyle(Themes::getLvglStyles().bg_color_error);

		switch (type)
		{
		case ResponseType::INFO:
			// No additional style
			break;
		case ResponseType::SUCCESS:
			addStyle(Themes::getLvglStyles().bg_color_success);
			break;
		case ResponseType::WARNING:
			addStyle(Themes::getLvglStyles().bg_color_warning);
			break;
		case ResponseType::ERROR:
			addStyle(Themes::getLvglStyles().bg_color_error);
			break;
		}
	}

	void MessageBox::onOkEvent(lv_event_t* e)
	{
		ZoneScoped;
		UI_LOCK();
		MessageBox* msgBox = static_cast<MessageBox*>(lv_event_get_user_data(e));
		msgBox->ok();
	}

	void MessageBox::onCancelEvent(lv_event_t* e)
	{
		ZoneScoped;
		UI_LOCK();
		MessageBox* msgBox = static_cast<MessageBox*>(lv_event_get_user_data(e));
		msgBox->cancel();
	}

	void MessageBox::onProgressTimer(lv_timer_t* timer)
	{
		ZoneScoped;
		UI_LOCK();
		MessageBox* msgBox = static_cast<MessageBox*>(lv_timer_get_user_data(timer));
		if (msgBox->m_progressCb)
		{
			size_t progress = msgBox->m_progressCb(msgBox);
			msgBox->setProgress(static_cast<int>(progress));
		}
	}
} // namespace UI
