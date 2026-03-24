#pragma once

#include "UI/Components/LVGL/generated/LvArc.gen.h"

namespace UI
{
	class LvArc : public LvArcGen
	{
	  public:
		using LvArcGen::LvArcGen;

		void setStyleArcWidth(lv_coord_t width, lv_part_t part = LV_PART_MAIN)
		{
			ZoneScoped;
			UI_LOCK();
			lv_obj_set_style_arc_width(getRootPtr(), width, part);
		}
	};
} // namespace UI
