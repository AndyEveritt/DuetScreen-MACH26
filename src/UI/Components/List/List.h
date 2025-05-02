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
			lv_obj_set_width(m_listCont, LV_PCT(100));
			lv_obj_set_flex_grow(m_listCont, 1);

			setLayout(Layout::VERTICAL);
		}

		void setTitle(const std::string& title)
		{
			UI_LOCK();
			lv_label_set_text(m_title, title.c_str());
		}

		void setLayout(Layout layout)
		{
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

		std::shared_ptr<T> getItem(const size_t index)
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
