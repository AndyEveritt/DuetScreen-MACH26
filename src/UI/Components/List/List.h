/*
 * List.h
 *
 *  Created on: 2025-05-02
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/View.h"
#include <memory>
#include <vector>

namespace UI
{
	class ListItem : public BaseView
	{
	  public:
		ListItem(const std::string& name, size_t index, lv_obj_t* parent)
			: BaseView(utils::format("%s_%u", name.c_str(), index), parent)
			, m_index(index)
		{
		}

		const size_t getIndex() const { return m_index; }

	  private:
		const size_t m_index;
	};

	template <typename T>
	class List : public BaseView
	{
	  public:
		// static_assert(std::is_base_of<ListItem, T>::value, "T must inherit from ListItem");

		enum class Layout
		{
			VERTICAL,
			HORIZONTAL
		};

		List(const std::string& name, lv_obj_t* parent)
			: BaseView(name, parent)
			, m_title(lv_label_create(getCont()))
			, m_listCont(lv_obj_create(getCont()))
		{
			lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);
			lv_obj_set_size(m_title, LV_PCT(100), LV_SIZE_CONTENT);
			lv_obj_set_size(m_listCont, LV_PCT(100), LV_PCT(100));
			lv_obj_set_flex_grow(m_listCont, 1);

			showTitle(false);
			setLayout(Layout::VERTICAL);
		}

		void setTitle(const std::string& title)
		{
			UI_LOCK();
			lv_label_set_text(m_title, title.c_str());
			showTitle(!title.empty());
		}

		void showTitle(bool show)
		{
			UI_LOCK();
			lv_obj_set_flag(m_title, LV_OBJ_FLAG_HIDDEN, !show);
		}

		void setListPad(lv_coord_t pad, lv_style_selector_t selector = LV_PART_MAIN, Padding type = Padding::ALL)
		{
			UI_LOCK();
			switch (type)
			{
			case Padding::ALL:
				lv_obj_set_style_pad_all(m_listCont, pad, selector);
				break;
			case Padding::LEFT:
				lv_obj_set_style_pad_left(m_listCont, pad, selector);
				break;
			case Padding::RIGHT:
				lv_obj_set_style_pad_right(m_listCont, pad, selector);
				break;
			case Padding::TOP:
				lv_obj_set_style_pad_top(m_listCont, pad, selector);
				break;
			case Padding::BOTTOM:
				lv_obj_set_style_pad_bottom(m_listCont, pad, selector);
				break;
			case Padding::COLUMN:
				lv_obj_set_style_pad_column(m_listCont, pad, selector);
				break;
			case Padding::ROW:
				lv_obj_set_style_pad_row(m_listCont, pad, selector);
				break;
			case Padding::HORIZONTAL:
				lv_obj_set_style_pad_hor(m_listCont, pad, selector);
				break;
			case Padding::VERTICAL:
				lv_obj_set_style_pad_ver(m_listCont, pad, selector);
				break;
			default:
				LOG_WARN("Unknown padding type");
				break;
			}
		}

		void setLayout(Layout layout)
		{
			UI_LOCK();
			switch (layout)
			{
			case Layout::VERTICAL:
				lv_obj_set_flex_flow(m_listCont, LV_FLEX_FLOW_COLUMN);
				break;
			case Layout::HORIZONTAL:
				lv_obj_set_flex_flow(m_listCont, LV_FLEX_FLOW_ROW);
				break;
			default:
				LOG_WARN("Unknown layout type");
				break;
			}
		}

		void setListGrow(const uint8_t grow)
		{
			UI_LOCK();
			lv_obj_set_flex_grow(m_listCont, grow);
		}

		void setListSize(const lv_coord_t w, const lv_coord_t h)
		{
			UI_LOCK();
			lv_obj_set_size(m_listCont, w, h);
		}

		template <typename... Args>
		void setItemCount(const size_t count, Args&&... args)
		{
			UI_LOCK();
			const size_t currentCount = getItemCount();
			if (count == currentCount)
			{
				return;
			}

			if (count < currentCount)
			{
				m_list.resize(count);
			}

			m_list.reserve(count);
			for (size_t i = currentCount; i < count; i++)
			{
				m_list.emplace_back(std::make_shared<T>(i, m_listCont, std::forward<Args>(args)...));
			}
		}
		const size_t getItemCount() const { return m_list.size(); }

		std::shared_ptr<T> getItem(const size_t index) const
		{
			UI_LOCK();
			if (index >= m_list.size())
			{
				return nullptr;
			}
			return m_list.at(index);
		}

	  private:
		lv_obj_t* m_title;
		lv_obj_t* m_listCont;

		std::vector<std::shared_ptr<T>> m_list;
	};
} // namespace UI
