#pragma once

#include "LockWrapper.h"
#include "Model.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/osal/lv_os.h"
#include <memory>

#if SIMULATION
#  define IMAGE_ASSET(name) "A:assets/" name
#  define VIDEO_ASSET(name) "assets/" name
#else
#  define IMAGE_ASSET(name) "A:/etc/assets/" name
#  define VIDEO_ASSET(name) "/etc/assets/" name
#endif

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
		BaseView(const std::string& name, BaseView* parent)
			: BaseView(name, parent->getCont())
		{
		}
		BaseView(const std::string& name, BaseView* parent, layout_t layout)
			: BaseView(name, parent->getCont(), layout)
		{
		}

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
		operator lv_obj_t*() const { return getCont(); }

		lv_obj_t* getScreen() const;
		lv_obj_t* getParent() const;
		lv_obj_t* getChild(int32_t id) const;
		uint32_t getChildCnt() const;
		layout_t getLayout();

		void setLayoutStyle(lv_layout_t style, lv_flex_flow_t flow = LV_FLEX_FLOW_ROW);
		void setLayout(layout_t layout);
		void setWidth(int widthPct);
		void setHeight(int heightPct);
		void setX(int xPct);
		void setY(int yPct);
		void setFlag(lv_obj_flag_t flag, bool enable);
		void setAlign(lv_align_t align, lv_coord_t x, lv_coord_t y);

		virtual void setStyle(lv_style_t* style, lv_style_selector_t selector);
		virtual void show();
		virtual void hide();
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
	template <class T, class BaseViewType = BaseView>
	class View : public BaseViewType
	{
		static_assert(std::is_base_of<BaseView, BaseViewType>::value, "BaseViewType must derive from BaseView");
		static_assert(std::is_base_of<BasePresenter, T>::value, "T must derive from Presenter");

	  public:
		View(const std::string& name, BaseView* parent)
			: BaseViewType(name, parent)
			, m_presenter(std::make_shared<T>(this))
		{
		}
		View(const std::string& name, BaseViewType* parent, layout_t layout)
			: BaseViewType(name, parent, layout)
			, m_presenter(std::make_shared<T>(this))
		{
		}

		View(const std::string& name, lv_obj_t* parent)
			: BaseViewType(name, parent)
			, m_presenter(std::make_shared<T>(this))
		{
		}
		View(const std::string& name, lv_obj_t* parent, layout_t layout)
			: BaseViewType(name, parent, layout)
			, m_presenter(std::make_shared<T>(this))
		{
		}
		View(const std::string& name, layout_t layout)
			: View(name, lv_screen_active(), layout)
		{
		}

		virtual ~View() { m_presenter->deactivate(); }

		/**
		 * @brief Get a pointer to the MVP model
		 */
		Model& getModel() const { return m_presenter->getModel(); }

		std::shared_ptr<T> getPresenter() { return m_presenter; }

		void activate()
		{
			Model::get().bind(m_presenter);
			m_presenter->activate();
		}
		void deactivate()
		{
			Model::get().unbind(m_presenter);
			m_presenter->deactivate();
		}

		/**
		 * @brief Shows the view by activating its presenter and then showing the view itself.
		 *
		 * @note This function calls the `onShow()` virtual method before showing the view.
		 */
		void show() override
		{
			activate();
			BaseViewType::show();
		}

		/**
		 * @brief Hides the view by deactivating its presenter and then hiding the view itself.
		 *
		 * @note This function calls the `onHide()` virtual method before hiding the view.
		 */
		void hide() override
		{
			deactivate();
			BaseViewType::hide();
		}

	  protected:
		std::shared_ptr<T> m_presenter;
	};

} // namespace UI

#define UI_LOCK()                                                                                                      \
	verbose("UI_LOCK requested by thread %u", std::this_thread::get_id());                                             \
	auto uiLock = ScopedLock(mutexUi);
