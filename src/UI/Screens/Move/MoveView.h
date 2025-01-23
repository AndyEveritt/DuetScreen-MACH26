#pragma once

#include "MovePresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/NumberPad/NumberPad.h"
#include "UI/Core/View.h"

namespace UI
{
	class MoveView;

	class AxisItem : public BaseView
	{
	  public:
		AxisItem(const size_t index, lv_obj_t* parent, layout_t layout);
		virtual ~AxisItem();

		const size_t getIndex() const { return m_index; }
		MoveView* getList() const { return m_list; }
		void setAxisLetter(const char* letter);
		void setHomed(const bool homed);
		void setToolPosition(const float& position);
		void setMachinePosition(const float& position);

	  private:
		static void onHomeEvent(lv_event_t* e);
		static void onRelMoveEvent(lv_event_t* e);

		size_t m_index;

		MoveView* m_list;

		Button m_home;
		Button m_relMove[8];
		lv_obj_t* m_toolPosition;
		lv_obj_t* m_machinePosition;
	};

	class MoveView : public View<MovePresenter>
	{
	  public:
		friend class AxisItem;

		MoveView(lv_obj_t* parent);

		const size_t getAxisCount() const { return m_axisItems.size(); }
		void setAxisCount(const size_t count);
		std::shared_ptr<AxisItem> getAxisItem(size_t index) const;

	  private:
		static void onHomeAllEvent(lv_event_t* e);
		static void onTrueBedLevelEvent(lv_event_t* e);
		static void onMeshBedLevelEvent(lv_event_t* e);
		static void onHeightmapEvent(lv_event_t* e);
		static void onDisableMotorsEvent(lv_event_t* e);
		static void onFeedRateEvent(lv_event_t* e);

		virtual void onShow() override;
		virtual void onHide() override;

		int32_t m_layoutColDsc[2];
		int32_t m_layoutRowDsc[5];

		lv_obj_t* m_topBarCont;
		lv_obj_t* m_listHeader;
		lv_obj_t* m_listCont;
		lv_obj_t* m_bottomBarCont;

		// Top Bar
		Button m_homeAll;
		Button m_trueBedLevel;
		Button m_meshBedLevel;
		Button m_heightmap;
		Button m_disableMotors;

		// List
		lv_obj_t* m_listHeaderPadding;
		lv_obj_t* m_toolPositionLabel;
		lv_obj_t* m_machinePositionLabel;
		std::vector<std::shared_ptr<AxisItem>> m_axisItems;

		// Bottom Bar
		lv_obj_t* m_feedRateLabel;
		Button m_feedRates[6];
	};
} // namespace UI