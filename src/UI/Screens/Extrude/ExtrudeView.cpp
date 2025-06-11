#include "ExtrudeView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	static uint32_t s_extrusionFeedRates[] = {50, 10, 5, 2, 1};
	static uint32_t s_extrusionFeedDistances[] = {100, 50, 20, 10, 5, 2, 1};
	static uint32_t s_selectedExtrusionFeedRateIndex = 2;
	static uint32_t s_selectedExtrusionFeedDistanceIndex = 2;

	ToolItem::ToolItem(const size_t index, lv_obj_t* parent, ExtrudeView& view)
		: ListItem("move_axis_item", index, parent)
		, m_selected(false)
		, m_list(view)
		, m_label(lv_label_create(getCont()))
		, m_heaters(utils::format("tool_%u_heaters", index), getCont())
		, m_filamentControls(lv_obj_create(getCont()))
		, m_filament(lv_dropdown_create(m_filamentControls))
		, m_unload(utils::format("extrude_unload_%u", index), m_filamentControls, _("unload"), layout_t(0, 0, 0, 100))
	{
		UI_LOCK();

		// Layout
		constexpr lv_coord_t pad = 2;
		lv_obj_set_size(getCont(), LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_style_pad_all(getCont(), pad, 0);
		lv_obj_set_style_pad_column(getCont(), pad, 0);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		uint8_t grow[] = {2, 5, 3}; // {label, heaters, filament controls}

		for (size_t i = 0; i < lv_obj_get_child_count(getCont()); i++)
		{
			lv_obj_t* obj = lv_obj_get_child(getCont(), i);
			if (obj == nullptr)
			{
				continue;
			}
			lv_obj_set_flex_grow(obj, grow[i]);
			lv_obj_set_height(obj, LV_SIZE_CONTENT);
			lv_obj_set_style_text_align(obj, i == 0 ? LV_TEXT_ALIGN_LEFT : LV_TEXT_ALIGN_CENTER, 0);
		}

		m_heaters.addStyle(Themes::getLvglStyles().no_border);
		m_heaters.addListStyle(Themes::getLvglStyles().no_border);
		lv_obj_add_style(m_filamentControls, Themes::getLvglStyles().no_border, 0);

		// Filament controls
		lv_obj_set_flex_flow(m_filamentControls, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_filamentControls, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_style_pad_all(m_filamentControls, pad, 0);
		lv_obj_set_style_pad_column(m_filamentControls, pad, 0);
		lv_obj_set_flex_grow(m_filament, 2);
		lv_obj_set_flex_grow(m_unload.getCont(), 1);
		lv_obj_set_height(m_filament, LV_SIZE_CONTENT);
		lv_obj_set_height(m_unload.getCont(), LV_SIZE_CONTENT);

		lv_obj_set_height(m_heaters, LV_SIZE_CONTENT);
		lv_obj_set_style_pad_all(m_heaters, pad, 0);
		m_heaters.setListSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_heaters.setListPad(0);
		m_heaters.setListGrow(0);

		// Callbacks
		lv_obj_add_flag(m_label, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_event_cb(m_label, onLabelEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_filament, onLoadFilamentEvent, LV_EVENT_VALUE_CHANGED, this);
		m_unload.setCallback(onUnloadEvent, LV_EVENT_CLICKED, this);

		m_unload.addStyle(Themes::getLvglStyles().actionBtn, 0);
	}

	ToolItem::~ToolItem() {}

	void ToolItem::setLabel(const char* txt)
	{
		UI_LOCK();
		lv_label_set_text(m_label, txt);
	}

	void ToolItem::setHeaterName(size_t index, const char* name)
	{
		UI_LOCK();
		auto heater = getHeater(index);
		if (heater == nullptr)
		{
			return;
		}
		lv_label_set_text(heater->label, name);
	}

	void ToolItem::setStatus(size_t index, const char* txt)
	{
		UI_LOCK();
		auto heater = getHeater(index);
		if (heater == nullptr)
		{
			return;
		}
		lv_label_set_text(heater->status, txt);
	}

	void ToolItem::setCurrentTemperature(size_t index, const float temp)
	{
		UI_LOCK();
		auto heater = getHeater(index);
		if (heater == nullptr)
		{
			return;
		}
		lv_label_set_text(heater->current, utils::format("%.1f", temp).c_str());
	}

	void ToolItem::setActiveTemperature(size_t index, const int32_t temp)
	{
		UI_LOCK();
		auto heater = getHeater(index);
		if (heater == nullptr)
		{
			return;
		}
		lv_label_set_text(heater->active, utils::format("%d", temp).c_str());
	}

	void ToolItem::setStandbyTemperature(size_t index, const int32_t temp)
	{
		UI_LOCK();
		auto heater = getHeater(index);
		if (heater == nullptr)
		{
			return;
		}
		lv_label_set_text(heater->standby, utils::format("%d", temp).c_str());
	}

	void ToolItem::setFilamentOptions(const std::vector<std::string>& options)
	{
		UI_LOCK();
		lv_dropdown_clear_options(m_filament);
		// lv_dropdown_add_option(m_filament, _("none"), LV_DROPDOWN_POS_LAST);
		for (const auto& option : options)
		{
			lv_dropdown_add_option(m_filament, option.c_str(), LV_DROPDOWN_POS_LAST);
		}
	}

	void ToolItem::showFilamentControls(bool show)
	{
		UI_LOCK();
		if (show)
		{
			lv_obj_remove_flag(m_filament, LV_OBJ_FLAG_HIDDEN);
			m_unload.show();
		}
		else
		{
			lv_obj_add_flag(m_filament, LV_OBJ_FLAG_HIDDEN);
			m_unload.hide();
		}
	}

	void ToolItem::setLoadedFilament(const char* filament)
	{
		UI_LOCK();
		int32_t index = lv_dropdown_get_option_index(m_filament, filament);
		if (index < 0)
		{
			if (filament[0] != '\0')
			{
				LOG_WARN("Failed to find filament option");
				lv_dropdown_set_text(m_filament, filament);
				lv_dropdown_set_selected_highlight(m_filament, false);
				return;
			}
			lv_dropdown_set_text(m_filament, _("none"));
			lv_dropdown_set_selected_highlight(m_filament, false);
			return;
		}
		lv_dropdown_set_text(m_filament, NULL);
		lv_dropdown_set_selected_highlight(m_filament, true);
		lv_dropdown_set_selected(m_filament, index);
	}

	size_t ToolItem::getHeaterCount() const
	{
		return m_heaters.getItemCount();
	}

	void ToolItem::setSelected(const bool selected)
	{
		if (m_selected == selected)
		{
			return;
		}
		UI_LOCK();
		lv_color_t color = lv_obj_get_style_bg_color(getCont(), LV_PART_MAIN);
		if (selected)
		{
			lv_obj_add_state(getCont(), LV_STATE_CHECKED);
		}
		else
		{
			lv_obj_remove_state(getCont(), LV_STATE_CHECKED);
		}
		m_selected = selected;
	}

	void ToolItem::setHeaterCount(const size_t count)
	{
		UI_LOCK();
		m_heaters.setItemCount(count, *this);
	}

	std::shared_ptr<ToolItem::Heater> ToolItem::getHeater(const size_t index)
	{
		return m_heaters.getItem(index);
	}

	ToolItem::Heater::Heater(const size_t index, lv_obj_t* parent, ToolItem& toolItem)
		: ListItem("extrude_heater", index, parent)
		, tool(toolItem)
		, labelCont(lv_obj_create(getCont()))
		, label(lv_label_create(labelCont))
		, status(lv_label_create(labelCont))
		, current(lv_label_create(getCont()))
		, active(lv_label_create(getCont()))
		, standby(lv_label_create(getCont()))
	{
		UI_LOCK();
		setStylePad(2, LV_PART_MAIN, Padding::ALL);
		setSize(LV_PCT(100), LV_SIZE_CONTENT);
		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		for (size_t i = 0; i < lv_obj_get_child_count(getCont()); i++)
		{
			lv_obj_t* obj = lv_obj_get_child(getCont(), i);
			lv_obj_set_height(obj, LV_SIZE_CONTENT);
			lv_obj_set_flex_grow(obj, 1);
			lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, 0);
		}
		lv_obj_set_flex_grow(labelCont, 2);

		lv_obj_set_flex_flow(labelCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(labelCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_style_pad_all(labelCont, 2, 0);
		for (size_t i = 0; i < lv_obj_get_child_count(labelCont); i++)
		{
			lv_obj_t* obj = lv_obj_get_child(labelCont, i);
			lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, 0);
			lv_obj_add_style(obj, Themes::getLvglStyles().pad_tiny, 0);
		}
		lv_obj_add_style(labelCont, Themes::getLvglStyles().actionBtn, 0);

		// Events
		lv_obj_add_flag(active, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_flag(standby, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_event_cb(labelCont, onStatusEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(active, onTemperaturesSetEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(standby, onTemperaturesSetEvent, LV_EVENT_CLICKED, this);

		// Styles
		lv_obj_add_style(active, Themes::getLvglStyles().input, 0);
		lv_obj_add_style(active, Themes::getLvglStyles().pad_zero, 0);

		lv_obj_add_style(standby, Themes::getLvglStyles().input, 0);
		lv_obj_add_style(standby, Themes::getLvglStyles().pad_zero, 0);

		lv_obj_set_height(active, LV_PCT(100));
		lv_obj_set_height(standby, LV_PCT(100));

		lv_obj_set_style_min_height(active, 30, 0);
		lv_obj_set_style_min_height(standby, 30, 0);
	}

	void ToolItem::onLabelEvent(lv_event_t* e)
	{
		UI_LOCK();
		ToolItem* item = static_cast<ToolItem*>(lv_event_get_user_data(e));
		item->getList().toggleToolState(item->getIndex());
	}

	void ToolItem::Heater::onStatusEvent(lv_event_t* e)
	{
		UI_LOCK();
		ToolItem::Heater* heater = static_cast<ToolItem::Heater*>(lv_event_get_user_data(e));
		heater->tool.getList().toggleHeaterState(heater->tool.getIndex(), heater->getIndex());
	}

	void ToolItem::Heater::onTemperaturesSetEvent(lv_event_t* e)
	{
		UI_LOCK();
		ToolItem::Heater* heater = static_cast<ToolItem::Heater*>(lv_event_get_user_data(e));
		lv_obj_t* obj = lv_event_get_target_obj(e);
		heater->tool.getList().m_presenter->configureNumberPad(
			heater->tool.getIndex(), heater->getIndex(), obj == heater->active);
		heater->tool.getList().showNumberPad(true);
	}

	void ToolItem::onLoadFilamentEvent(lv_event_t* e)
	{
		UI_LOCK();
		ToolItem* item = static_cast<ToolItem*>(lv_event_get_user_data(e));
		char selectedFilament[MAX_FILAMENT_NAME_LENGTH];
		lv_dropdown_get_selected_str(item->m_filament, selectedFilament, sizeof(selectedFilament));
		item->getList().loadFilament(item->getIndex(), selectedFilament);
	}

	void ToolItem::onUnloadEvent(lv_event_t* e)
	{
		UI_LOCK();
		ToolItem* item = static_cast<ToolItem*>(lv_event_get_user_data(e));
		item->getList().unloadFilament(item->getIndex());
	}

	ExtrudeView::ExtrudeView(lv_obj_t* parent)
		: View(lv_obj_create, "move_view", parent, layout_t(0, 0, 100, 100))
		, m_layoutColDsc{LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST}
		, m_layoutRowDsc{30, LV_GRID_FR(4), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_listHeader(lv_obj_create(getCont()))
		, m_listCont(lv_obj_create(getCont()))
		, m_bottomBarCont(lv_obj_create(getCont()))
		, m_headerTool(lv_label_create(m_listHeader))
		, m_headerStatus(lv_label_create(m_listHeader))
		, m_headerCurrent(lv_label_create(m_listHeader))
		, m_headerActive(lv_label_create(m_listHeader))
		, m_headerStandby(lv_label_create(m_listHeader))
		, m_headerFilament(lv_label_create(m_listHeader))
		, m_headerPad(lv_obj_create(m_listHeader))
		, m_toolItems("extrude_tool_items", m_listCont)
		, m_feedDistCont(lv_obj_create(m_bottomBarCont))
		, m_feedRateCont(lv_obj_create(m_bottomBarCont))
		, m_extrudeControlCont(lv_obj_create(m_bottomBarCont))
		, m_feedDistLabel(lv_label_create(m_feedDistCont))
		, m_feedDistListCont(lv_obj_create(m_feedDistCont))
		, m_feedDists{Button("extrude_feed_dist_1", m_feedDistListCont, "", layout_t(0, 0, 0, 100)),
					  Button("extrude_feed_dist_2", m_feedDistListCont, "", layout_t(0, 0, 0, 100)),
					  Button("extrude_feed_dist_3", m_feedDistListCont, "", layout_t(0, 0, 0, 100)),
					  Button("extrude_feed_dist_4", m_feedDistListCont, "", layout_t(0, 0, 0, 100)),
					  Button("extrude_feed_dist_5", m_feedDistListCont, "", layout_t(0, 0, 0, 100)),
					  Button("extrude_feed_dist_6", m_feedDistListCont, "", layout_t(0, 0, 0, 100)),
					  Button("extrude_feed_dist_7", m_feedDistListCont, "", layout_t(0, 0, 0, 100))}
		, m_feedRateLabel(lv_label_create(m_feedRateCont))
		, m_feedRateListCont(lv_obj_create(m_feedRateCont))
		, m_feedRates{Button("extrude_feed_rate_1", m_feedRateListCont, "", layout_t(0, 0, 0, 100)),
					  Button("extrude_feed_rate_2", m_feedRateListCont, "", layout_t(0, 0, 0, 100)),
					  Button("extrude_feed_rate_3", m_feedRateListCont, "", layout_t(0, 0, 0, 100)),
					  Button("extrude_feed_rate_4", m_feedRateListCont, "", layout_t(0, 0, 0, 100)),
					  Button("extrude_feed_rate_5", m_feedRateListCont, "", layout_t(0, 0, 0, 100))}
		, m_retract("extrude_retract", m_extrudeControlCont, _("retract"), layout_t(0, 0, 100, 0))
		, m_extrude("extrude_extrude", m_extrudeControlCont, _("extrude"), layout_t(0, 0, 100, 0))
		, m_numberPad("extrude_number_pad", getCont(), layout_t(65, 0, 35, 100))
	{
		UI_LOCK();

		lv_obj_add_style(m_listHeader, Themes::getLvglStyles().bg_color_header, 0);

		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(getCont(), m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(m_listHeader, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_listCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_set_grid_cell(m_bottomBarCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
		lv_obj_set_grid_cell(m_numberPad.getCont(), LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 3);

		// List Header
		lv_obj_set_flex_flow(m_listHeader, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_listHeader, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_style_pad_ver(m_listHeader, 0, 0);
		lv_obj_set_style_pad_column(m_listHeader, 5, 0);

		static const char* headerLabels[] = {"toollist_tool",
											 "toollist_status",
											 "toollist_current",
											 "toollist_active",
											 "toollist_standby",
											 "toollist_filament",
											 ""};

		uint8_t s_listGrow[] = {2, 2, 1, 1, 1, 2, 1};

		static_assert(ARRAY_SIZE(s_listGrow) == ARRAY_SIZE(headerLabels), "Invalid array size");
		if (ARRAY_SIZE(s_listGrow) != lv_obj_get_child_cnt(m_listHeader))
		{
			LOG_FATAL_THROW("Invalid s_listGrow array size");
		}

		for (size_t i = 0; i < ARRAY_SIZE(s_listGrow); i++)
		{
			lv_obj_t* obj = lv_obj_get_child(m_listHeader, i);
			if (obj == nullptr)
			{
				continue;
			}
			lv_obj_set_flex_grow(obj, s_listGrow[i]);
			lv_obj_set_style_text_align(obj, i == 0 ? LV_TEXT_ALIGN_LEFT : LV_TEXT_ALIGN_CENTER, 0);
			lv_obj_set_height(obj, LV_SIZE_CONTENT);
			const char* labelID = headerLabels[i];
			if (labelID[0] != '\0')
			{
				lv_label_set_text(obj, _(labelID));
			}
		}
		lv_obj_set_height(m_headerPad, 0); // effectively hides it

		// List
		lv_obj_set_style_pad_all(m_listCont, 0, 0);
		lv_obj_set_size(m_toolItems, LV_PCT(100), LV_PCT(100));
		m_toolItems.setListPad(0);

		// Bottom Bar
		lv_obj_set_style_pad_all(m_bottomBarCont, 5, 0);
		lv_obj_set_style_pad_column(m_bottomBarCont, 5, 0);
		lv_obj_set_flex_flow(m_bottomBarCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_bottomBarCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_feedDistCont, ARRAY_SIZE(m_feedDists));
		lv_obj_set_flex_grow(m_feedRateCont, ARRAY_SIZE(m_feedRates));
		lv_obj_set_flex_grow(m_extrudeControlCont, 2);
		lv_obj_set_height(m_feedDistCont, LV_PCT(100));
		lv_obj_set_height(m_feedRateCont, LV_PCT(100));
		lv_obj_set_height(m_extrudeControlCont, LV_PCT(100));
		lv_obj_set_style_pad_all(m_feedDistCont, 2, 0);
		lv_obj_set_style_pad_all(m_feedRateCont, 2, 0);
		lv_obj_set_style_pad_all(m_extrudeControlCont, 2, 0);

		// Feed Distance
		lv_obj_set_flex_flow(m_feedDistCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_grow(m_feedDistListCont, 1);
		lv_label_set_text(m_feedDistLabel, _("extrude_feed_dist"));

		lv_obj_set_width(m_feedDistListCont, LV_PCT(100));
		lv_obj_set_flex_flow(m_feedDistListCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_style_pad_all(m_feedDistListCont, 2, 0);
		lv_obj_set_style_pad_column(m_feedDistListCont, 2, 0);
		for (Button& feedDist : m_feedDists)
		{
			feedDist.setText(utils::format("%u", s_extrusionFeedDistances[&feedDist - m_feedDists]).c_str());
			feedDist.setCheckable(true);
			feedDist.setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(&feedDist - m_feedDists)));
			feedDist.setCallback(onFeedDistEvent, LV_EVENT_CLICKED, this);
			lv_obj_set_flex_grow(feedDist.getCont(), 1);
		}
		s_selectedExtrusionFeedDistanceIndex =
			StorageHelper::getData(ID_EXTRUSION_SELECTED_DISTANCE, s_selectedExtrusionFeedDistanceIndex);
		m_feedDists[s_selectedExtrusionFeedDistanceIndex].setChecked(true);

		// Feed Rates
		lv_obj_set_flex_flow(m_feedRateCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_grow(m_feedRateListCont, 1);
		lv_label_set_text(m_feedRateLabel, _("extrude_feed_rate"));

		lv_obj_set_width(m_feedRateListCont, LV_PCT(100));
		lv_obj_set_flex_flow(m_feedRateListCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_style_pad_all(m_feedRateListCont, 2, 0);
		lv_obj_set_style_pad_column(m_feedRateListCont, 2, 0);
		for (Button& feedRate : m_feedRates)
		{
			feedRate.setText(utils::format("%u", s_extrusionFeedRates[&feedRate - m_feedRates]).c_str());
			feedRate.setCheckable(true);
			feedRate.setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(&feedRate - m_feedRates)));
			feedRate.setCallback(onFeedRateEvent, LV_EVENT_CLICKED, this);
			lv_obj_set_flex_grow(feedRate.getCont(), 1);
		}
		s_selectedExtrusionFeedRateIndex =
			StorageHelper::getData(ID_EXTRUSION_SELECTED_FEEDRATE, s_selectedExtrusionFeedRateIndex);
		m_feedRates[s_selectedExtrusionFeedRateIndex].setChecked(true);

		// Extrusion Control
		lv_obj_set_flex_flow(m_extrudeControlCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_grow(m_retract.getCont(), 1);
		lv_obj_set_flex_grow(m_extrude.getCont(), 1);
		lv_obj_set_style_pad_all(m_extrudeControlCont, 2, 0);
		lv_obj_set_style_pad_row(m_extrudeControlCont, 2, 0);

		m_retract.setCallback(onRetractEvent, LV_EVENT_CLICKED, this);
		m_extrude.setCallback(onExtrudeEvent, LV_EVENT_CLICKED, this);

		m_retract.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_extrude.addStyle(Themes::getLvglStyles().actionBtn, 0);
	}

	void ExtrudeView::setToolCount(const size_t count)
	{
		UI_LOCK();
		m_toolItems.setItemCount(count, *this);
	}

	std::shared_ptr<ToolItem> ExtrudeView::getExtruderItem(const size_t index) const
	{
		UI_LOCK();
		return m_toolItems.getItem(index);
	}

	void ExtrudeView::toggleToolState(size_t index)
	{
		m_presenter->toggleToolState(index);
	}

	void ExtrudeView::toggleHeaterState(size_t toolIndex, size_t heaterIndex)
	{
		m_presenter->toggleHeaterState(toolIndex, heaterIndex);
	}

	void ExtrudeView::loadFilament(size_t index, const char* filament)
	{
		m_presenter->loadFilament(index, filament);
	}

	void ExtrudeView::unloadFilament(size_t index)
	{
		m_presenter->unloadFilament(index);
	}

	void ExtrudeView::showNumberPad(bool show)
	{
		if (show)
		{
			UI_LOCK();
			m_numberPad.clear();
			openScreen(&m_numberPad, false);
		}
		else
		{
			closeScreen(&m_numberPad);
		}
	}

	void ExtrudeView::onRetractEvent(lv_event_t* e)
	{
		UI_LOCK();
		ExtrudeView* view = static_cast<ExtrudeView*>(lv_event_get_user_data(e));
		auto dist = s_extrusionFeedDistances[s_selectedExtrusionFeedDistanceIndex];
		auto rate = s_extrusionFeedRates[s_selectedExtrusionFeedRateIndex];
		view->m_presenter->retract(dist, rate);
	}

	void ExtrudeView::onExtrudeEvent(lv_event_t* e)
	{
		UI_LOCK();
		ExtrudeView* view = static_cast<ExtrudeView*>(lv_event_get_user_data(e));
		auto dist = s_extrusionFeedDistances[s_selectedExtrusionFeedDistanceIndex];
		auto rate = s_extrusionFeedRates[s_selectedExtrusionFeedRateIndex];
		view->m_presenter->extrude(dist, rate);
	}

	void ExtrudeView::onFeedDistEvent(lv_event_t* e)
	{
		UI_LOCK();
		ExtrudeView* view = static_cast<ExtrudeView*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
		view->m_feedDists[s_selectedExtrusionFeedDistanceIndex].setChecked(false);
		s_selectedExtrusionFeedDistanceIndex = reinterpret_cast<uintptr_t>(lv_obj_get_user_data(btn));
		view->m_feedDists[s_selectedExtrusionFeedDistanceIndex].setChecked(true);
		StorageHelper::setData(ID_EXTRUSION_SELECTED_DISTANCE, s_selectedExtrusionFeedDistanceIndex);
	}

	void ExtrudeView::onFeedRateEvent(lv_event_t* e)
	{
		UI_LOCK();
		ExtrudeView* view = static_cast<ExtrudeView*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
		view->m_feedRates[s_selectedExtrusionFeedRateIndex].setChecked(false);
		s_selectedExtrusionFeedRateIndex = reinterpret_cast<uintptr_t>(lv_obj_get_user_data(btn));
		view->m_feedRates[s_selectedExtrusionFeedRateIndex].setChecked(true);
		StorageHelper::setData(ID_EXTRUSION_SELECTED_FEEDRATE, s_selectedExtrusionFeedRateIndex);
	}

	void ExtrudeView::onShow()
	{
		m_numberPad.hide();
	}
	void ExtrudeView::onHide() {}
} // namespace UI
