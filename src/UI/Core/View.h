#pragma once

#include "Model.h"
#include "lvgl/lvgl.h"
#include <memory>

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
		BaseView(const std::string& name, lv_obj_t* parent);

		BaseView(const std::string& name, lv_obj_t* parent, layout_t layout)
			: BaseView(name, parent)
		{
			lv_obj_set_pos(getCont(), lv_pct(layout.x), lv_pct(layout.y));
			lv_obj_set_width(getCont(), layout.w == LV_SIZE_CONTENT ? LV_SIZE_CONTENT : lv_pct(layout.w));
			lv_obj_set_height(getCont(), layout.h == LV_SIZE_CONTENT ? LV_SIZE_CONTENT : lv_pct(layout.h));
		}

		BaseView(const std::string& name, layout_t layout)
			: BaseView(name, lv_scr_act(), layout)
		{
		}

		virtual ~BaseView();

		inline const char* getName() const { return m_name.c_str(); }
		/**
		 * @return Get the base container for the view
		 */
		inline lv_obj_t* getCont() const { return m_cont; }
		lv_obj_t* getScreen() const;
		lv_obj_t* getParent() const;
		lv_obj_t* getChild(int32_t id) const;
		uint32_t getChildCnt() const;
		layout_t getLayout();

		void setLayout(layout_t layout);
		void setWidth(int widthPct);
		void setHeight(int heightPct);
		void setX(int xPct);
		void setY(int yPct);

		virtual void setStyle(lv_style_t* style, lv_style_selector_t selector);
		void show();
		void hide();
		bool isVisible();
		virtual bool back();

	  protected:
		virtual void init() {}
		virtual void onShow() {}
		virtual void onHide() {}
		virtual void refresh() {}

	  private:
		lv_obj_t* m_cont;
		std::string m_name;
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
		View(const std::string& name, lv_obj_t* parent)
			: BaseView(name, parent)
			, m_presenter(this)
		{
		}
		View(const std::string& name, lv_obj_t* parent, layout_t layout)
			: BaseView(name, parent, layout)
			, m_presenter(this)
		{
		}
		View(const std::string& name, layout_t layout)
			: View(name, lv_scr_act(), layout)
		{
		}

		virtual ~View() { m_presenter.deactivate(); }

		/**
		 * @brief Get a pointer to the MVP model
		 */
		Model* getModel() { return m_presenter->getModel(); }

		void activate() { m_presenter.activate(); }
		void deactivate() { m_presenter.deactivate(); }

		void show()
		{
			activate();
			BaseView::show();
		}
		void hide()
		{
			deactivate();
			BaseView::hide();
		}

	  protected:
		T m_presenter;
	};
} // namespace UI
