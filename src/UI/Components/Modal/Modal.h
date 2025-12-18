/*
 * Modal.h
 *
 *  Created on: 2025-07-25
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvObj.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include <concepts>

namespace UI
{
	template <typename T>
		requires(std::is_base_of_v<LvObj, T>)
	class Modal : public T
	{
	  public:
		using modal_base_marker = void; // tag to identify Modal-derived types
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
			m_modalBg.addEventCallback(
				[this](lv_event_t*)
				{
					if (!isBlocking())
						close();
				},
				LV_EVENT_CLICKED);
			m_modalBg.addStyle(Themes::getLvglStyles().bg_modal);
		}

		void open() { openModal(this); }
		void close() { closeModal(this); }

		void setParent(LvObj& parent) { m_modalBg.setParent(parent); }

		void setBlocking(bool blocking) { m_blocking = blocking; }
		bool isBlocking() const { return m_blocking; }

	  private:
		/**
		 * @warning private to prevent accidental calls (use this->open() or openModal() instead)
		 */
		void show(bool move_to_front = false) override { T::show(move_to_front); }

		/**
		 * @warning private to prevent accidental calls (use this->close() or closeModal() instead)
		 */
		void hide(bool move_to_front = false) override { T::hide(move_to_front); }

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
		bool m_blocking = false;
	};
} // namespace UI
