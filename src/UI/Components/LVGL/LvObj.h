/*
 * LvObj.h
 *
 *  Created on: 2025-06-10
 *      Author: Andy Everitt
 */

#pragma once

#include "LockWrapper.h"
#include "lvgl/lvgl.h"

namespace UI
{
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

	using lv_create_t = lv_obj_t* (*)(lv_obj_t * parent);

	class LvObj
	{
	  public:
		LvObj(lv_create_t initFunc, const std::string& name, lv_obj_t* parent);
		LvObj(lv_create_t initFunc, const std::string& name, lv_obj_t* parent, layout_t layout);
		LvObj(lv_create_t initFunc, const std::string& name, layout_t layout)
			: LvObj(initFunc, name, lv_screen_active(), layout)
		{
		}

		// Delete copy constructor and assignment operator
		LvObj(const LvObj&) = delete;
		LvObj& operator=(const LvObj&) = delete;

		virtual ~LvObj();

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

		void setUserData(void* user_data);
		void* getUserData() const;

		bool hasFlag(lv_obj_flag_t flag) const;
		bool hasState(lv_state_t state) const;

		void setLayoutStyle(lv_layout_t style);
		void setFlexGrow(uint8_t grow);
		void setFlexFlow(lv_flex_flow_t flow);
		void setFlexAlign(lv_flex_align_t main, lv_flex_align_t cross, lv_flex_align_t mid);
		void setGridDsc(const int32_t col_dsc[], const int32_t row_dsc[]);
		void setGridCell(lv_obj_t* obj,
						 lv_grid_align_t x_align,
						 int32_t col_pos,
						 int32_t col_span,
						 lv_grid_align_t y_align,
						 int32_t row_pos,
						 int32_t row_span);
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
		void setState(lv_state_t state, bool enable);
		void setAlign(lv_align_t align, lv_coord_t x, lv_coord_t y);

		/* Styling */

		void addStyle(const lv_style_t* style,
					  const lv_style_selector_t selector = LV_PART_MAIN,
					  bool recursive = false);
		void setStylePad(lv_coord_t pad, lv_style_selector_t selector = LV_PART_MAIN, Padding type = Padding::ALL);
		void setStyleBgColor(lv_color_t color, lv_style_selector_t selector = LV_PART_MAIN);
		void setStyleTextAlign(lv_text_align_t align, lv_style_selector_t selector = LV_PART_MAIN);

		void addEventCallback(lv_event_cb_t cb, lv_event_code_t code, void* userData);

		void setVisibile(bool display) { display ? show() : hide(); }
		virtual void show();
		virtual void hide();
		bool isVisible();
		virtual bool back();

	  protected:
		virtual void onShow() {}
		virtual void onHide() {}
		virtual void refresh() {}

	  private:
		lv_obj_t* m_cont;
		std::string m_name;
	};
} // namespace UI

#define UI_LOCK()                                                                                                      \
	LOG_VERBOSE("UI_LOCK requested by thread {}", Log::GetThreadId());                                                 \
	auto uiLock = ScopedLock(mutexUi);
