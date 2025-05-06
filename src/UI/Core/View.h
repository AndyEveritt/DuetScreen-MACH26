#pragma once

#include "LockWrapper.h"
#include "Model.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/osal/lv_os.h"
#include <fmt/ostream.h>
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

	enum class Padding
	{
		ALL,
		HORIZONTAL,
		VERTICAL,
		TOP,
		BOTTOM,
		LEFT,
		RIGHT,
		COLUMN,
		ROW,
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
		BaseView(const std::string& name, lv_obj_t* parent, layout_t layout);
		BaseView(const std::string& name, layout_t layout)
			: BaseView(name, lv_scr_act(), layout)
		{
		}

		virtual ~BaseView();

		inline const std::string& getName() const { return m_name; }
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
		void setFlexGrow(uint8_t grow);
		void setLayout(layout_t layout);
		void setWidth(lv_coord_t width);
		void setHeight(lv_coord_t height);
		void setSize(lv_coord_t width, lv_coord_t height);
		void setMinWidth(lv_coord_t width, lv_style_selector_t selector = LV_PART_MAIN);
		void setMinHeight(lv_coord_t height, lv_style_selector_t selector = LV_PART_MAIN);
		void setX(lv_coord_t x);
		void setY(lv_coord_t y);
		void setPos(lv_coord_t x, lv_coord_t y);
		void setFlag(lv_obj_flag_t flag, bool enable);
		void setAlign(lv_align_t align, lv_coord_t x, lv_coord_t y);
		void setPad(lv_coord_t pad, lv_style_selector_t selector = LV_PART_MAIN, Padding type = Padding::ALL);

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

		virtual ~View()
		{
			deactivate();
			if (m_presenter)
			{
				m_presenter->setView(nullptr);
			}
		}

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
	LOG_VERBOSE("UI_LOCK requested by thread {}", Log::GetThreadId());                                                 \
	auto uiLock = ScopedLock(mutexUi);
