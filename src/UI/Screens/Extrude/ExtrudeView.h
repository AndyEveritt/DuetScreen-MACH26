#pragma once

#include "ExtrudePresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/List/List.h"
#include "UI/Components/ToolList/ToolList.h"
#include "UI/Core/View.h"

namespace UI
{
	class ExtrudeView;

	class ToolItem : public ListItem
	{
	  public:
		ToolItem(const size_t index, lv_obj_t* parent, ExtrudeView& view);
		virtual ~ToolItem();

		ExtrudeView& getList() const { return m_list; }
		void setLabel(const char* name);
		void setSelected(const bool selected);
		void setHeaterCount(const size_t count);
		size_t getHeaterCount() const;
		void setHeaterName(size_t index, const char* name);
		void setStatus(size_t index, const char* status);
		void setCurrentTemperature(size_t index, const float temperature);
		void setActiveTemperature(size_t index, const int32_t temperature);
		void setStandbyTemperature(size_t index, const int32_t temperature);
		void showFilamentControls(bool show);
		void setLoadedFilament(const char* filament);
		void setFilamentOptions(const std::vector<std::string>& options);

	  private:
		static void onLabelEvent(lv_event_t* e);
		static void onLoadFilamentEvent(lv_event_t* e);
		static void onUnloadEvent(lv_event_t* e);

		class Heater : public ListItem
		{
		  public:
			Heater(const size_t index, lv_obj_t* parent, ToolItem& toolItem);

			ToolItem& tool;
			lv_obj_t* labelCont;
			lv_obj_t* label;
			lv_obj_t* status;
			lv_obj_t* current;
			lv_obj_t* active;
			lv_obj_t* standby;

		  private:
			static void onStatusEvent(lv_event_t* e);
			static void onTemperaturesSetEvent(lv_event_t* e);

			lv_style_t m_targetTempStyle;
		};

		std::shared_ptr<Heater> getHeater(const size_t index);

		bool m_selected;

		ExtrudeView& m_list;

		lv_obj_t* m_label;
		List<Heater> m_heaters;
		lv_obj_t* m_filamentControls;
		lv_obj_t* m_filament;
		Button m_unload;

		lv_style_t m_listStyle;
	};

	class ExtrudeView : public View<ExtrudePresenter>
	{
	  public:
		friend class ToolItem;
		friend class ExtrudePresenter;

		ExtrudeView(lv_obj_t* parent);

		const size_t getToolCount() const { return m_toolItems.getItemCount(); }
		void setToolCount(const size_t count);
		std::shared_ptr<ToolItem> getExtruderItem(const size_t index) const;

		void toggleToolState(size_t toolIndex);
		void toggleHeaterState(size_t toolIndex, size_t heaterIndex);
		void loadFilament(size_t toolIndex, const char* filament);
		void unloadFilament(size_t toolIndex);

	  private:
		void showNumberPad(bool show);

		static void onFeedDistEvent(lv_event_t* e);
		static void onFeedRateEvent(lv_event_t* e);
		static void onExtrudeEvent(lv_event_t* e);
		static void onRetractEvent(lv_event_t* e);

		virtual void onShow() override;
		virtual void onHide() override;

		int32_t m_layoutColDsc[2];
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
		List<ToolItem> m_toolItems;

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

		// Number Pad
		ToolListNumPad m_numberPad;
	};
} // namespace UI