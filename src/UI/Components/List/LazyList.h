/*
 * LazyList.h
 *
 *  Created on: 2025-12-01
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Styles/Styles.h"
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace UI
{
	// Concept to validate a list item constructor for T
	// Parameter order: F (callable), then T (constructed type)
	template <typename F, typename T>
	concept LazyListItemConstructor = requires(F&& f, size_t index, LvObj& parent) {
		{ std::invoke(std::forward<F>(f), index) } -> std::same_as<T>;
	};

	template <typename T>
		requires(std::is_base_of_v<LvObj, T>)
	class LazyObj
	{
	  public:
		using ObjT = T;

		/**
		 * @brief Calculate the size (height) of this item.
		 * @return Height in pixels.
		 */
		virtual int32_t getSize() const = 0;

		/**
		 * @brief Create the LVGL object for this item.
		 * @param index The index of the item.
		 * @param parent The parent object (the list).
		 * @return Pointer to the created object.
		 */
		std::unique_ptr<ObjT> create(size_t index, LvObj& parent)
		{
			ZoneScoped;
			auto obj = std::make_unique<ObjT>(fmt::format("lazy_item_{:d}", index), parent);
			obj->setFlag(LV_OBJ_FLAG_SCROLLABLE, false);
			return obj;
		}

		/**
		 * @brief Update the LVGL object with data from this LazyObj.
		 * @param index The index of the LazyObj.
		 * @param obj The LVGL object to update (created by create()).
		 */
		virtual void update(size_t index, ObjT& obj) = 0;

		/**
		 * @brief Called to determine if the item is visible and should be considered when rendering the list
		 * @return `true` if the item is visible
		 */
		virtual bool isVisible() const { return true; }
	};

	template <typename LazyT>
	class LazyList : public LvContainer
	{
		using LazyTPtr = std::unique_ptr<LazyT>;
		using LazyTRef = LazyT&;
		using T = LazyT::ObjT;
		using TPtr = std::unique_ptr<T>;
		using TRef = T&;

	  public:
		LazyList(const std::string& name, LvObj& parent)
			: LvContainer(name, parent)
			, m_header("header", getRoot())
			, m_title("title", m_header)
			, m_listCont("list", getRoot())
			, m_sizer(lv_obj_create, "sizer", m_listCont)
		{
			ZoneScoped;
			// Setup layout similar to List
			setFlexFlow(LV_FLEX_FLOW_COLUMN);

			m_header.setMinWidth(LV_SIZE_CONTENT);
			m_header.setSize(LV_PCT(100), LV_SIZE_CONTENT);
			m_header.setFlexFlow(LV_FLEX_FLOW_ROW);
			m_header.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
			m_header.addStyle(Themes::getLvglStyles().bg_color_header, LV_PART_MAIN);

			// List container (scrollable)
			m_listCont.setSize(LV_PCT(100), LV_PCT(100));
			m_listCont.setFlexGrow(1);
			m_listCont.setScrollDir(LV_DIR_VER);

			m_title.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			showTitle(false);

			// Sizer for scrolling
			m_sizer.setSize(1, 1);
			m_sizer.setFlag(LV_OBJ_FLAG_CLICKABLE, false);
			m_sizer.setFlag(LV_OBJ_FLAG_CLICK_FOCUSABLE, false);

			// Events
			addEventCallback(
				[this](lv_event_t*)
				{
					auto width = lv_obj_get_style_clamped_width(getRootPtr());
					auto height = lv_obj_get_style_clamped_height(getRootPtr());

					m_listCont.setWidth(width == LV_SIZE_CONTENT ? LV_SIZE_CONTENT : LV_PCT(100));
					m_listCont.setHeight(height == LV_SIZE_CONTENT ? LV_SIZE_CONTENT : LV_PCT(100));
					m_listCont.setFlexGrow(height == LV_SIZE_CONTENT ? 0 : 1);
				},
				LV_EVENT_SIZE_CHANGED);
			m_listCont.addEventCallback([this](lv_event_t*) { updateVisibleItems(); }, LV_EVENT_SCROLL);
			m_listCont.addEventCallback([this](lv_event_t*) { updateVisibleItems(); }, LV_EVENT_SIZE_CHANGED);
		}

		// List Interface methods
		LvContainer& getHeader() { return m_header; }
		LvLabel& getTitle() { return m_title; }
		LvContainer& getListContainer() { return m_listCont; }

		auto& getVisibleItems() { return m_items; }
		auto& getLazyItems() { return m_pool; }

		size_t getVisibleItemCount() const { return m_items.size(); }
		size_t getLazyItemCount() const { return m_pool.size(); }

		auto getVisibleItem(size_t index) const { return m_items.at(index).get(); }
		auto getLazyItem(size_t index) const { return m_pool.at(index).get(); }

		void setTitle(std::string_view title)
		{
			ZoneScoped;
			UI_LOCK();
			m_title.setText(title);
			showTitle(!title.empty());
		}

		void showHeader(bool show)
		{
			ZoneScoped;
			UI_LOCK();
			m_header.setFlag(LV_OBJ_FLAG_HIDDEN, !show);
		}

		void showTitle(bool show)
		{
			ZoneScoped;
			UI_LOCK();
			m_title.setFlag(LV_OBJ_FLAG_HIDDEN, !show);
			if (show)
			{
				showHeader(true);
			}
			else if (m_header.getChildCount() <= 1)
			{
				showHeader(false);
			}
		}

		void clear()
		{
			ZoneScoped;
			m_items.clear();
			m_pool.clear();
			m_sizer.setHeight(0);
		}

		template <typename F>
			requires LazyListItemConstructor<F, LazyTPtr>
		size_t setItemCount(const size_t count, F&& constructor)
		{
			ZoneScoped;
			UI_LOCK();
			const size_t currentCount = m_pool.size();
			if (count == currentCount)
			{
				return 0;
			}

			if (count < currentCount)
			{
				m_pool.resize(count);
			}
			else
			{
				m_pool.reserve(count);
				for (size_t i = currentCount; i < count; i++)
				{
					m_pool.emplace_back(std::invoke(constructor, i));
				}
			}
			refresh();
			return count > currentCount ? count - currentCount : 0;
		}

		void refresh()
		{
			ZoneScoped;
			int32_t totalHeight = 0;
			int32_t itemPad = m_listCont.getStyleProp(LV_STYLE_PAD_ROW).num;
			bool first = true;
			for (const auto& item : m_pool)
			{
				if (!item->isVisible())
				{
					continue;
				}
				totalHeight += item->getSize();
				if (!first)
				{
					totalHeight += itemPad;
				}
				first = false;
			}
			m_sizer.setHeight(totalHeight);
			updateLayout();
			updateVisibleItems();
		}

	  protected:
		void updateVisibleItems()
		{
			ZoneScoped;
			lv_coord_t scrollTop = m_listCont.getScrollTop();
			lv_coord_t listHeight = m_listCont.getHeight();
			lv_coord_t scrollBottom = scrollTop + listHeight;
			int32_t itemPad = m_listCont.getStyleProp(LV_STYLE_PAD_ROW).num;

			struct item_info_t
			{
				size_t index;
				lv_coord_t top;
				lv_coord_t height;
			};

			std::vector<item_info_t> visibleItems;
			lv_coord_t itemStart = 0;
			for (size_t i = 0; i < m_pool.size(); ++i)
			{
				const LazyTPtr& item = m_pool.at(i);
				if (!item->isVisible())
				{
					continue;
				}

				lv_coord_t itemHeight = item->getSize();
				lv_coord_t itemBottom = itemStart + itemHeight;

				if (itemBottom >= scrollTop && itemStart <= scrollBottom)
				{
					visibleItems.emplace_back(i, itemStart, itemHeight);
				}
				itemStart += itemHeight + itemPad;
			}

			// Iterate visible indices
			m_items.resize(visibleItems.size());
			for (size_t i = 0; i < visibleItems.size(); ++i)
			{
				item_info_t info = visibleItems[i]; // index in m_pool

				LazyTRef lazyObj = *m_pool.at(info.index);
				TPtr& objPtr = m_items.at(i);

				if (!objPtr)
				{
					objPtr = std::move(lazyObj.create(info.index, m_listCont));
				}
				objPtr->setHeight(info.height);
				lazyObj.update(info.index, *objPtr);

				objPtr->setY(info.top);
			}
		}

	  private:
		LvContainer m_header;
		LvLabel m_title;
		LvContainer m_listCont;
		LvObj m_sizer;

		std::vector<TPtr> m_items;	  // the actual lvgl objects for visible items
		std::vector<LazyTPtr> m_pool; // pool of all lazy objects
	};

} // namespace UI
