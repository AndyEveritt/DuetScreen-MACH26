#pragma once

#include "lvgl/lvgl.h"
#include "model.h"

namespace UI
{
	class BasePresenter;

	class BaseView
	{
	  public:
		BaseView(lv_obj_t* parent, lv_coord_t xPct, lv_coord_t yPct, lv_coord_t widthPct, lv_coord_t heightPct)
			: m_cont(lv_obj_create(parent))
		{
			lv_obj_set_pos(m_cont, lv_pct(xPct), lv_pct(yPct));
			lv_obj_set_size(m_cont, lv_pct(widthPct), lv_pct(heightPct));
		}
		BaseView(lv_coord_t xPct, lv_coord_t yPct, lv_coord_t widthPct, lv_coord_t heightPct)
			: BaseView(lv_scr_act(), xPct, yPct, widthPct, heightPct)
		{
		}
		BaseView(lv_obj_t* parent)
			: BaseView(parent, 0, 0, 100, 100)
		{
		}

		/**
		 * @return Get the base container for the view
		 */
		lv_obj_t* getBaseContainer() { return m_cont; }

	  protected:
		virtual void init() {}
		virtual void onShow() {}
		virtual void onHide() {}
		virtual void refresh() {}

		lv_obj_t* m_cont;
	};

	/**
	 * This is the base View, each screen should inherit from this class. It provides a link
	 * to the Presenter class.
	 *
	 * @tparam T The type of Presenter associated with this view.
	 *
	 * @note All views in the application must be a subclass of this type.
	 */
	template <class T>
	class View : public BaseView
	{
		static_assert(std::is_base_of<BasePresenter, T>::value, "T must derive from Presenter");

	  public:
		View(lv_obj_t* parent, lv_coord_t xPct, lv_coord_t yPct, lv_coord_t widthPct, lv_coord_t heightPct)
			: BaseView(parent, xPct, yPct, widthPct, heightPct)
			, m_presenter(this)
		{
		}
		View(lv_coord_t xPct, lv_coord_t yPct, lv_coord_t widthPct, lv_coord_t heightPct)
			: View(lv_scr_act(), xPct, yPct, widthPct, heightPct)
		{
		}
		View(lv_obj_t* parent)
			: View(parent, 0, 0, 100, 100)
		{
		}

		/**
		 * @brief Get a pointer to the MVP model
		 */
		Model* getModel() { return m_presenter->getModel(); }

		void show()
		{
			m_presenter.activate();
			onShow();
		}
		void hide()
		{
			m_presenter.deactivate();
			onHide();
		}

	  protected:
		T m_presenter;
	};
} // namespace UI
