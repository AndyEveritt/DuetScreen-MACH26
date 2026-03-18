/*
 * ObjectCancelView.h
 *
 *  Created on: 2025-03-18
 *      Author: Andy Everitt
 */

#pragma once

#include "ObjectCancelPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Canvas/Canvas.h"
#include "UI/Components/List/List.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Components/Modal/Modal.h"
#include "UI/Core/View.h"
#include "ObjectModel/Job.h"
#include <memory>
#include <string>
#include <vector>

namespace UI
{
	class ObjectCancelView;

	class ObjectItem : public ListItem
	{
	  public:
		ObjectItem(size_t index, LvObj& parent, ObjectCancelView& view);
		void update(std::string_view name, bool isCurrent, bool isCancelled);

	  private:
		ObjectCancelView& m_view;
		LvLabel m_label;
	};

	class ObjectCancelView : public View<ObjectCancelPresenter>
	{
	  public:
		ObjectCancelView(const std::string& name, LvObj& parent);
		virtual ~ObjectCancelView();

		// Canvas
		void setAxisRange(Canvas::range_t xRange, Canvas::range_t yRange);
		void beginObjectOverlayUpdate();
		void updateObjectOverlay(size_t index, const OM::JobObject::Point& bounds, bool isCurrent, bool isCancelled);

		// Object list
		void setCurrentObjectName(std::string_view name);
		void setObjectCount(size_t count);
		void updateObjectItem(size_t index, std::string_view name, bool isCurrent, bool isCancelled);

		// Modal
		void showConfirmModal(size_t index, std::string_view name, bool isCancelled);

		// Cancel current button state
		void setCancelCurrentEnabled(bool enabled);

		Canvas& getCanvas() { return m_canvas; }
		auto& getConfirmModal() { return m_confirmModal; }
		Button& getCancelCurrentButton() { return m_cancelCurrentBtn; }

	  private:
		struct OverlayRect
		{
			size_t index = 0;
			ObjectCancelView* owner = nullptr;
			std::unique_ptr<LvContainer> rect;
		};

		void onInit() override;
		void onShow() override;

		static void onCancelCurrentClicked(lv_event_t* e);
		static void onObjectOverlayPressed(lv_event_t* e);

		OverlayRect* ensureOverlayRect(size_t index);

		int32_t m_layoutColDsc[3] = {LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		int32_t m_layoutRowDsc[3] = {LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

		Canvas m_canvas{"object_canvas", getRoot()};
		LvLabel m_currentObjectLabel{"current_object_label", m_canvas.getHeader()};
		List<ObjectItem> m_objectList{"object_list", getRoot()};
		Button m_cancelCurrentBtn{"cancel_current", getRoot()};
		std::vector<std::unique_ptr<OverlayRect>> m_overlayRects;

		Modal<MessageBox> m_confirmModal{"confirm_object_cancel", getRoot(), layout_t(0, 0, 60, LV_SIZE_CONTENT)};
	};
} // namespace UI
