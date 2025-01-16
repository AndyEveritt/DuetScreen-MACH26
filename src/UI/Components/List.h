#pragma once

#include "UI/Components/Button.h"
#include "UI/Core/View.h"
#include "lvgl/lvgl.h"
#include "utils/utils.h"
#include <memory>
#include <vector>

namespace UI
{
	class DynamicList : public BaseView
	{
	  public:
		DynamicList(const char* name, lv_obj_t* parent, layout_t layout)
			: BaseView(name, parent, layout)
		{
			lv_obj_set_layout(getCont(), LV_LAYOUT_FLEX);
			lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW_WRAP);
		}

		virtual void setStyle(lv_style_t* style, lv_style_selector_t selector) override {}

		void setItemCnt(size_t cnt)
		{
			size_t currentCnt = getItemCnt();
			if (cnt <= currentCnt)
			{
				while (m_items.size() > cnt)
				{
					m_items.pop_back();
				}
				return;
			}

			for (size_t i = currentCnt; i < cnt; i++)
			{
				m_items.emplace_back(std::make_shared<Button>(utils::format("list_btn_%u", i).c_str(),
															  getCont(),
															  utils::format("Button %u", i).c_str(),
															  layout_t(0, 0, 20, 20)));
			}
		}
		size_t getItemCnt() const { return m_items.size(); }

		std::shared_ptr<Button> getButton(size_t index) const
		{
			if (index >= m_items.size())
			{
				return nullptr;
			}
			return m_items.at(index);
		}

	  private:
		std::vector<std::shared_ptr<Button>> m_items;
	};
} // namespace UI