/*
 * List.h
 *
 *  Created on: 2025-05-02
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Styles/Styles.h"
#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace UI
{
	class ListItem : public LvContainer
	{
	  public:
		ListItem(size_t index, LvObj& parent)
			: LvContainer(fmt::format("{}", index), parent)
			, m_index(index)
		{
			// addStyle(Themes::getLvglStyles().bg_color_list_item, 0);
			// addStyle(Themes::getLvglStyles().bg_color_secondary, LV_STATE_CHECKED);
		}

		size_t getIndex() const { return m_index; }

	  private:
		const size_t m_index;
	};

	template <typename T>
	// requires(std::is_base_of_v<LvObj, T>)
	class List : public LvObj
	{
		using TPtr = std::unique_ptr<T>;
		using TRef = T&;

	  public:
		List(const std::string& name, LvObj& parent)
			: LvObj(lv_obj_create, name, parent)
			, m_header("header", getRoot())
			, m_title("title", m_header)
			, m_listCont("list", getRoot())
		{
			setFlexFlow(LV_FLEX_FLOW_COLUMN);

			m_header.setMinWidth(LV_SIZE_CONTENT);
			m_header.setSize(LV_PCT(100), LV_SIZE_CONTENT);
			m_header.setFlexFlow(LV_FLEX_FLOW_ROW);
			m_header.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
			m_header.addStyle(Themes::getLvglStyles().bg_color_header);
			m_header.addStyle(Themes::getComponentStyles().list_header);

			m_listCont.setFlexFlow(LV_FLEX_FLOW_COLUMN);
			m_listCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
			m_listCont.setSize(LV_PCT(100), LV_SIZE_CONTENT);

			m_title.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			m_title.addStyle(Themes::getComponentStyles().list_title);

			showTitle(false);
		}

		LvContainer& getHeader() { return m_header; }
		LvLabel& getTitle() { return m_title; }
		LvContainer& getListContainer() { return m_listCont; }

		void setTitle(std::string_view title)
		{
			UI_LOCK();
			m_title.setText(title);
			showTitle(!title.empty());
		}

		void showHeader(bool show)
		{
			UI_LOCK();
			m_header.setFlag(LV_OBJ_FLAG_HIDDEN, !show);
		}

		void showTitle(bool show)
		{
			UI_LOCK();
			m_title.setFlag(LV_OBJ_FLAG_HIDDEN, !show);
			if (show)
			{
				showHeader(true);
			}
			else if (m_header.getChildCount() <= 1) // Only the title is present
			{
				showHeader(false);
			}
		}

		void setListPad(lv_coord_t pad, lv_style_selector_t selector = LV_PART_MAIN, Padding type = Padding::ALL)
		{
			m_listCont.setStylePad(pad, selector, type);
		}

		void setListFlow(lv_flex_flow_t flow) { m_listCont.setFlexFlow(flow); }

		void setListGrow(const uint8_t grow) { m_listCont.setFlexGrow(grow); }

		void addListStyle(const lv_style_t* style, lv_style_selector_t selector = LV_PART_MAIN)
		{
			m_listCont.addStyle(style, selector);
		}

		void setListSize(const lv_coord_t w, const lv_coord_t h) { m_listCont.setSize(w, h); }

		void clear() { m_list.clear(); }

		TRef addItem()
		{
			UI_LOCK();
			LOG_DBG("Adding item to list \"{:s}\"", getName());
			auto item = std::make_unique<T>(getItemCount(), m_listCont);
			m_list.push_back(std::move(item));
			return *m_list.back();
		}

		TRef addItem(std::function<TPtr(size_t, LvObj&)> constructor)
		{
			UI_LOCK();
			LOG_DBG("Adding item to list \"{:s}\"", getName());
			auto item = constructor(getItemCount(), m_listCont);
			m_list.push_back(std::move(item));
			return *m_list.back();
		}

		size_t setItemCount(const size_t count, std::function<TPtr(size_t, LvObj&)> constructor)
		{
			UI_LOCK();
			const size_t currentCount = getItemCount();
			if (count == currentCount)
			{
				return 0;
			}

			if (count < currentCount)
			{
				LOG_DBG("Resizing list \"{:s}\" down from {:d} to {:d}", getName(), currentCount, count);
				m_list.resize(count);
			}
			else
			{
				LOG_DBG("Resizing list \"{:s}\" up from {:d} to {:d}", getName(), currentCount, count);
				m_list.reserve(count);
				for (size_t i = currentCount; i < count; i++)
				{
					m_list.emplace_back(constructor(i, m_listCont));
				}
			}

			return count > currentCount ? count - currentCount : 0;
		}

		template <typename F, typename = std::enable_if_t<std::is_invocable_r_v<TPtr, F, size_t, LvObj&>>>
			requires std::is_constructible_v<std::function<TPtr(size_t, LvObj&)>, F>
		size_t setItemCount(const size_t count, F&& constructor)
		{
			return setItemCount(count, std::function<TPtr(size_t, LvObj&)>(std::forward<F>(constructor)));
		}

		template <typename Class, typename... Args>
		size_t setItemCount(size_t count,
							Class* instance,
							TPtr (Class::*constructor)(const size_t index, LvObj& parent, Args...),
							Args&&... args)
		{
			UI_LOCK();
			const size_t currentCount = getItemCount();
			if (count == currentCount)
			{
				return 0;
			}

			if (count < currentCount)
			{
				LOG_DBG("Resizing list \"{:s}\" down from {:d} to {:d}", getName(), currentCount, count);
				m_list.resize(count);
			}
			else
			{
				LOG_DBG("Resizing list \"{:s}\" up from {:d} to {:d}", getName(), currentCount, count);
				m_list.reserve(count);
				for (size_t i = currentCount; i < count; i++)
				{
					m_list.emplace_back((instance->*constructor)(i, m_listCont, std::forward<Args>(args)...));
				}
			}

			return count > currentCount ? count - currentCount : 0;
		}

		template <typename... Args,
				  typename = std::enable_if_t<sizeof...(Args) != 1 ||
											  !std::is_invocable_r_v<TPtr, std::decay_t<Args>..., size_t, LvObj&>>>
			requires std::is_constructible_v<T, size_t, LvObj&, Args...>
		size_t setItemCount(const size_t count, Args&&... args)
		{
			UI_LOCK();
			const size_t currentCount = getItemCount();
			if (count == currentCount)
			{
				return 0;
			}

			if (count < currentCount)
			{
				LOG_DBG("Resizing list \"{:s}\" down from {:d} to {:d}", getName(), currentCount, count);
				m_list.resize(count);
			}
			else
			{
				LOG_DBG("Resizing list \"{:s}\" up from {:d} to {:d}", getName(), currentCount, count);
				m_list.reserve(count);
				for (size_t i = currentCount; i < count; i++)
				{
					m_list.emplace_back(std::make_unique<T>(i, m_listCont, std::forward<Args>(args)...));
				}
			}

			return count > currentCount ? count - currentCount : 0;
		}

		size_t getItemCount() const { return m_list.size(); }

		T* getItem(const size_t index) const
		{
			UI_LOCK();
			if (index >= m_list.size())
			{
				return nullptr;
			}
			return m_list.at(index).get();
		}

		const std::vector<TPtr>& getItems() const { return m_list; }

		void iterateListItems(const std::function<void(size_t index, TRef item)>& func)
		{
			UI_LOCK();
			for (size_t i = 0; i < m_list.size(); i++)
			{
				auto& item = m_list.at(i);
				if (!item)
				{
					continue;
				}
				func(i, *item);
			}
		}

		auto begin() { return m_list.begin(); }
		auto end() { return m_list.end(); }
		auto begin() const { return m_list.begin(); }
		auto end() const { return m_list.end(); }

	  private:
		LvContainer m_header;
		LvLabel m_title;
		LvContainer m_listCont;

		std::vector<TPtr> m_list;
	};
} // namespace UI
