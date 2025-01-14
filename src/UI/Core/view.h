#pragma once

#include "lvgl/lvgl.h"
#include "model.h"

namespace UI
{
	class Lock
	{
	  public:
		Lock() { lv_lock(); }
		~Lock() { lv_unlock(); }
	};

	/**
	 * @brief Position and size layout in percentage 0-100
	 */
	struct layout_t
	{
		lv_coord_t x;
		lv_coord_t y;
		lv_coord_t w;
		lv_coord_t h;
	};

	class BasePresenter;

	class BaseView
	{
	  public:
		BaseView(const char* name, lv_obj_t* parent)
			: m_cont(lv_obj_create(parent))
			, m_name(name)
		{
		}

		BaseView(const char* name, lv_obj_t* parent, layout_t layout)
			: BaseView(name, parent)
		{
			lv_obj_set_pos(m_cont, lv_pct(layout.x), lv_pct(layout.y));
			lv_obj_set_size(m_cont, lv_pct(layout.w), lv_pct(layout.h));
			lv_obj_set_style_pad_all(m_cont, 5, 0);
		}

		BaseView(const char* name, layout_t layout)
			: BaseView(name, lv_scr_act(), layout)
		{
		}

		virtual ~BaseView();

		const char* getName() const { return m_name; }
		/**
		 * @return Get the base container for the view
		 */
		lv_obj_t* getCont() { return m_cont; }
		lv_obj_t* getScreen() const;
		lv_obj_t* getParent() const;
		lv_obj_t* getChild(int32_t id) const;
		uint32_t getChildCnt() const;
		layout_t getLayout();

		virtual void setStyle(lv_style_t* style, lv_style_selector_t selector);

	  protected:
		virtual void init() {}
		virtual void onShow() {}
		virtual void onHide() {}
		virtual void refresh() {}

		lv_obj_t* m_cont;
		const char* m_name;
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
		View(const char* name, lv_obj_t* parent, layout_t layout)
			: BaseView(name, parent, layout)
			, m_presenter(this)
		{
		}
		View(const char* name, layout_t layout)
			: View(name, lv_scr_act(), layout)
		{
		}
		View(const char* name, lv_obj_t* parent)
			: View(name, parent, layout_t(0, 0, 100, 100))
		{
		}

		virtual ~View() { m_presenter.deactivate(); }

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
