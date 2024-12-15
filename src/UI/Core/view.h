#pragma once

#include "lvgl/lvgl.h"
#include "model.h"

namespace UI
{
	/**
	 * This is the base View, each screen should inherit from this class. It provides a link
	 * to the Presenter class.
	 *
	 * @tparam T The type of Presenter associated with this view.
	 *
	 * @note All views in the application must be a subclass of this type.
	 */
	template <class T>
	class View
	{
	  public:
		View(lv_obj_t* parent, lv_coord_t xPct, lv_coord_t yPct, lv_coord_t widthPct, lv_coord_t heightPct)
			: m_cont(lv_obj_create(parent))
			, m_presenter(0)
		{
			lv_obj_set_pos(m_cont, lv_pct(xPct), lv_pct(yPct));
			lv_obj_set_size(m_cont, lv_pct(widthPct), lv_pct(heightPct));
		}
		View(lv_coord_t xPct, lv_coord_t yPct, lv_coord_t widthPct, lv_coord_t heightPct)
			: View(lv_scr_act(), xPct, yPct, widthPct, heightPct)
		{
		}
		View(lv_obj_t* parent)
			: View(parent, 0, 0, 100, 100)
		{
		}
		virtual ~View() {}

		/**
		 * @brief Binds an instance of a specific Presenter type (subclass) to the View instance. This function is
		 * called automatically when a new presenter/view pair is activated.
		 * @param newPresenter The specific Presenter to be associated with the View.
		 */
		void bind(T& newPresenter) { m_presenter = &newPresenter; }

		/**
		 * @brief Get a pointer to the MVP model
		 */
		Model* getModel() { return m_presenter->getModel(); }

		/**
		 * @return Get the base container for the view
		 */
		lv_obj_t* getBaseContainer() { return m_cont; }

		virtual void init() {}
		virtual void onShow() {}
		virtual void onHide() {}
		virtual void refresh() {}

		void show();
		void hide();
		void back();

	  protected:
		lv_obj_t* m_cont;
		T* m_presenter;
	};
} // namespace UI
