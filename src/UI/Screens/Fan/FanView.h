#pragma once

#include "FanPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Slider.h"
#include "UI/Core/View.h"

namespace UI
{
	class FanView : public View<FanPresenter>
	{
	  public:
		class FanItem : LvObj
		{
		  public:
			FanItem(size_t index, FanView& view);

			void setLabel(const char* label);
			void setValue(uint32_t value);

		  private:
			static void onFanOffClicked(lv_event_t* e);
			static void onFanMaxClicked(lv_event_t* e);

			// Fans
			FanView& m_view;
			size_t m_index;
			Button m_off;
			Slider m_slider;
			Button m_max;
		};

		FanView(lv_obj_t* parent);

		size_t getFanCount() const { return m_fans.size(); }
		void setFanCount(size_t count);
		void setFanLabel(size_t index, const char* label);
		void setFanValue(size_t index, uint32_t value);

	  private:
		lv_obj_t* m_fanHeader;
		std::vector<std::shared_ptr<FanItem>> m_fans;
	};
} // namespace UI