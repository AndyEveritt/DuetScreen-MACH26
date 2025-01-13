#pragma once

#include "lvgl/lvgl.h"

namespace UI
{
	class Component
	{
	  public:
		Component(const char* name, lv_obj_t* parent);
		virtual ~Component();

		const char* getName() const;
		lv_obj_t* getScreen() const;
		lv_obj_t* getParent() const;
		lv_obj_t* getObj() const { return m_obj; }
		lv_obj_t* getChild(int32_t id) const;
		uint32_t getChildCnt() const;
		virtual void setStyle(lv_style_t* style, lv_style_selector_t selector) = 0;

	  protected:
		const char* m_name;
		lv_obj_t* m_obj;
	};
} // namespace UI
