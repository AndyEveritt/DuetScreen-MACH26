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
		View(lv_coord_t xPct = 0, lv_coord_t yPct = 0, lv_coord_t widthPct = 100, lv_coord_t heightPct = 100)
			: m_cont(lv_obj_create(lv_scr_act()))
			, m_presenter(0)
		{
			lv_obj_set_pos(m_cont, lv_pct(xPct), lv_pct(yPct));
			lv_obj_set_size(m_cont, lv_pct(widthPct), lv_pct(heightPct));
		}
		virtual ~View() {}

		/**
		 * Binds an instance of a specific Presenter type (subclass) to the View instance. This function
		 * is called automatically when a new presenter/view pair is activated.
		 *
		 * @param [in] newPresenter The specific Presenter to be associated with the View.
		 */
		void bind(T& newPresenter) { m_presenter = &newPresenter; }
		Model* getModel() { return m_presenter->getModel(); }
		lv_obj_t* getBaseContainer() { return m_cont; }

		virtual void init() {}
		virtual void onShow() {}
		virtual void onHide() {}

		void show();
		void hide();
		void back();

	  protected:
		lv_obj_t* m_cont;
		T* m_presenter;
	};
} // namespace UI
