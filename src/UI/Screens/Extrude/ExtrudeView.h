#pragma once

#include "ExtrudePresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/NumberPad/NumberPad.h"
#include "UI/Core/View.h"

namespace UI
{
	class ExtrudeView;

	class ExtruderItem : public BaseView
	{
	  public:
		ExtruderItem(const size_t index, lv_obj_t* parent, layout_t layout);
		virtual ~ExtruderItem();

		const size_t getIndex() const { return m_index; }
		ExtrudeView* getList() const { return m_list; }
		void setLabel(const char* name);
		void setStatus(const char* status);
		void setSelected(const bool selected);
		void setCurrentTemperature(const float& temperature);
		void setActiveTemperature(const float& temperature);
		void setStandbyTemperature(const float& temperature);
		void setLoadedFilament(const char* filament);
		void setFilamentOptions(const std::vector<std::string>& options);

	  private:
		static void onLabelEvent(lv_event_t* e);
		static void onStatusEvent(lv_event_t* e);
		static void onTemperaturesSetEvent(lv_event_t* e);
		static void onLoadFilamentEvent(lv_event_t* e);
		static void onUnloadEvent(lv_event_t* e);

		size_t m_index;

		ExtrudeView* m_list;

		lv_obj_t* m_label;
		lv_obj_t* m_status;
		lv_obj_t* m_currentTemp;
		lv_obj_t* m_activeTemp;
		lv_obj_t* m_standbyTemp;
		lv_obj_t* m_filament;
		Button m_unload;
	};

	class ExtrudeView : public View<ExtrudePresenter>
	{
	  public:
		friend class ExtruderItem;

		ExtrudeView(lv_obj_t* parent);

		const size_t getToolCount() const { return m_toolItems.size(); }
		void setToolCount(const size_t count);
		std::shared_ptr<ExtruderItem> getExtruderItem(size_t index) const;

	  private:
		static void onFeedDistEvent(lv_event_t* e);
		static void onFeedRateEvent(lv_event_t* e);
		static void onExtrudeEvent(lv_event_t* e);
		static void onRetractEvent(lv_event_t* e);

		virtual void onShow() override;
		virtual void onHide() override;

		int32_t m_layoutColDsc[4];
		int32_t m_layoutRowDsc[4];

		lv_obj_t* m_listHeader;
		lv_obj_t* m_listCont;
		lv_obj_t* m_bottomBarCont;

		// List
		lv_obj_t* m_headerTool;
		lv_obj_t* m_headerStatus;
		lv_obj_t* m_headerCurrent;
		lv_obj_t* m_headerActive;
		lv_obj_t* m_headerStandby;
		lv_obj_t* m_headerFilament;
		lv_obj_t* m_headerPad;
		std::vector<std::shared_ptr<ExtruderItem>> m_toolItems;

		// Bottom Container
		lv_obj_t* m_feedDistCont;
		lv_obj_t* m_feedRateCont;
		lv_obj_t* m_extrudeControlCont;

		// Feed Dists
		lv_obj_t* m_feedDistLabel;
		lv_obj_t* m_feedDistListCont;
		Button m_feedDists[7];

		// Feed Rates
		lv_obj_t* m_feedRateLabel;
		lv_obj_t* m_feedRateListCont;
		Button m_feedRates[5];

		// Extrusion Control
		Button m_retract;
		Button m_extrude;
	};
} // namespace UI