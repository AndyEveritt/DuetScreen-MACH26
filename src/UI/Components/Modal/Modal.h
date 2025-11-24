/*
 * Modal.h
 *
 *  Created on: 2025-07-25
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/LvObj.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	template <typename T>
		requires(std::is_base_of_v<LvObj, T>)
	class Modal : public T
	{
	  public:
		template <typename... Args>
			requires(std::is_constructible_v<T, const std::string&, LvObj&, Args...>)
		Modal(const std::string& name, LvObj& parent, Args&&... args)
			: T(name, parent, std::forward<Args>(args)...)
			, m_modalBg("modal_bg", parent, layout_t(0, 0, 100, 100))
		{
			UI_LOCK();

			this->addStyle(Themes::getLvglStyles().card);
			this->addStyle(Themes::getLvglStyles().modal);

			this->setAlign(LV_ALIGN_CENTER, 0, 0);
			static_cast<T*>(this)->setParent(m_modalBg); // make sure to call the base setParent
			this->hide();

			m_modalBg.setFlag(LV_OBJ_FLAG_FLOATING, true);
			m_modalBg.addEventCallback(modalBgEventHandler, LV_EVENT_CLICKED, this);
			m_modalBg.addStyle(Themes::getLvglStyles().bg_modal);
		}

		void close()
		{
			if (closeModal(this) || closeScreen(this, false))
			{
				return;
			}
			this->hide();
		}

		void setParent(LvObj& parent) { m_modalBg.setParent(parent); }

	  private:
		static void modalBgEventHandler(lv_event_t* e)
		{
			UI_LOCK();

			Modal* modal = (Modal*)lv_event_get_user_data(e);
			if (modal)
			{
				modal->close();
			}
		}

		void onShow() override
		{
			m_modalBg.show(true);
			T::onShow();
		}
		void onHide() override
		{
			T::onHide();
			m_modalBg.hide();
		}

		LvContainer m_modalBg;
	};
} // namespace UI
