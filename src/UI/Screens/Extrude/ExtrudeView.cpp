#include "ExtrudeView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	static uint32_t s_extrusionFeedRates[] = {50, 10, 5, 2, 1};
	static uint32_t s_extrusionFeedDistances[] = {100, 50, 20, 10, 5, 2, 1};
	static uint32_t s_selectedExtrusionFeedRateIndex = 2;
	static uint32_t s_selectedExtrusionFeedDistanceIndex = 2;

	static uint8_t s_listGrow[] = {2, 1, 1, 1, 1, 2, 1};

	ExtruderItem::ExtruderItem(const size_t index, lv_obj_t* parent, layout_t layout)
		: BaseView(utils::format("move_axis_item_%u", index), parent, layout)
		, m_index(index)
		, m_label(lv_label_create(getCont()))
		, m_status(lv_label_create(getCont()))
		, m_currentTemp(lv_label_create(getCont()))
		, m_activeTemp(lv_textarea_create(getCont()))
		, m_standbyTemp(lv_textarea_create(getCont()))
		, m_filament(lv_dropdown_create(getCont()))
		, m_unload(utils::format("extrude_unload_%u", index), getCont(), _("unload"), layout_t(0, 0, 0, 100))
	{
		// Layout
		constexpr lv_coord_t pad = 2;
		lv_obj_set_style_pad_all(getCont(), pad, 0);
		lv_obj_set_style_pad_column(getCont(), pad, 0);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		for (size_t i = 0; i < ARRAY_SIZE(s_listGrow); i++)
		{
			lv_obj_t* obj = lv_obj_get_child(getCont(), i);
			if (obj == nullptr)
			{
				continue;
			}
			lv_obj_set_flex_grow(obj, s_listGrow[i]);
			lv_obj_set_height(obj, LV_SIZE_CONTENT);
			lv_obj_set_style_text_align(obj, i == 0 ? LV_TEXT_ALIGN_LEFT : LV_TEXT_ALIGN_CENTER, 0);
		}
		lv_textarea_set_one_line(m_activeTemp, true);
		lv_textarea_set_one_line(m_standbyTemp, true);
		lv_textarea_set_accepted_chars(m_activeTemp, "0123456789");
		lv_textarea_set_accepted_chars(m_standbyTemp, "0123456789");
		lv_textarea_set_max_length(m_activeTemp, 4);
		lv_textarea_set_max_length(m_standbyTemp, 4);

		m_unload.setCallback(onUnloadEvent, LV_EVENT_CLICKED, this);
	}

	ExtruderItem::~ExtruderItem() {}

	void ExtruderItem::setLabel(const char* txt)
	{
		Lock lock;
		lv_label_set_text(m_label, txt);
	}

	void ExtruderItem::setStatus(const char* txt)
	{
		Lock lock;
		lv_label_set_text(m_status, txt);
	}

	void ExtruderItem::setCurrentTemperature(const float& temp)
	{
		Lock lock;
		lv_label_set_text_fmt(m_currentTemp, "%.1f", temp);
	}

	void ExtruderItem::setActiveTemperature(const float& temp)
	{
		Lock lock;
		lv_textarea_set_text(m_activeTemp, utils::format("%.1f", temp).c_str());
	}

	void ExtruderItem::setStandbyTemperature(const float& temp)
	{
		Lock lock;
		lv_textarea_set_text(m_standbyTemp, utils::format("%.1f", temp).c_str());
	}

	void ExtruderItem::setFilamentOptions(const std::vector<std::string>& options)
	{
		Lock lock;
		lv_dropdown_clear_options(m_filament);
		lv_dropdown_add_option(m_filament, _("none"), LV_DROPDOWN_POS_LAST);
		for (const auto& option : options)
		{
			lv_dropdown_add_option(m_filament, option.c_str(), LV_DROPDOWN_POS_LAST);
		}
	}

	void ExtruderItem::setLoadedFilament(const char* filament)
	{
		Lock lock;
		int32_t index = lv_dropdown_get_option_index(m_filament, filament);
		if (index < 0)
		{
			if (filament[0] != '\0')
			{
				warn("Failed to find filament option");
				return;
			}
			index = 0;
		}
		lv_dropdown_set_selected(m_filament, index);
	}

	void ExtruderItem::onUnloadEvent(lv_event_t* e) {}

	ExtrudeView::ExtrudeView(lv_obj_t* parent)
		: View("move_view", parent, layout_t(0, 0, 100, 100))
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
	{
		Lock lock;

		// Layout
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_listCont, 4);
		lv_obj_set_flex_grow(m_bottomBarCont, 1);
		lv_obj_set_size(m_listHeader, LV_PCT(100), 30);
		lv_obj_set_width(m_listCont, LV_PCT(100));
		lv_obj_set_width(m_bottomBarCont, LV_PCT(100));

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

		static_assert(ARRAY_SIZE(s_listGrow) == ARRAY_SIZE(headerLabels), "Invalid array size");
		if (ARRAY_SIZE(s_listGrow) != lv_obj_get_child_cnt(m_listHeader))
		{
			fatal("Invalid s_listGrow array size");
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
		lv_obj_set_flex_flow(m_listCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_style_pad_row(m_listCont, 2, 0);

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
	}

	void ExtrudeView::onRetractEvent(lv_event_t* e)
	{
		Lock lock;
		ExtrudeView* view = static_cast<ExtrudeView*>(lv_event_get_user_data(e));
		auto dist = s_extrusionFeedDistances[s_selectedExtrusionFeedDistanceIndex];
		auto rate = s_extrusionFeedRates[s_selectedExtrusionFeedRateIndex];
		view->m_presenter.retract(dist, rate);
	}

	void ExtrudeView::onExtrudeEvent(lv_event_t* e)
	{
		Lock lock;
		ExtrudeView* view = static_cast<ExtrudeView*>(lv_event_get_user_data(e));
		auto dist = s_extrusionFeedDistances[s_selectedExtrusionFeedDistanceIndex];
		auto rate = s_extrusionFeedRates[s_selectedExtrusionFeedRateIndex];
		view->m_presenter.extrude(dist, rate);
	}

	void ExtrudeView::onFeedDistEvent(lv_event_t* e)
	{
		Lock lock;
		ExtrudeView* view = static_cast<ExtrudeView*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
		view->m_feedDists[s_selectedExtrusionFeedDistanceIndex].setChecked(false);
		s_selectedExtrusionFeedDistanceIndex = reinterpret_cast<uintptr_t>(lv_obj_get_user_data(btn));
		view->m_feedDists[s_selectedExtrusionFeedDistanceIndex].setChecked(true);
		StorageHelper::setData(ID_EXTRUSION_SELECTED_DISTANCE, s_selectedExtrusionFeedDistanceIndex);
	}

	void ExtrudeView::onFeedRateEvent(lv_event_t* e)
	{
		Lock lock;
		ExtrudeView* view = static_cast<ExtrudeView*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
		view->m_feedRates[s_selectedExtrusionFeedRateIndex].setChecked(false);
		s_selectedExtrusionFeedRateIndex = reinterpret_cast<uintptr_t>(lv_obj_get_user_data(btn));
		view->m_feedRates[s_selectedExtrusionFeedRateIndex].setChecked(true);
		StorageHelper::setData(ID_EXTRUSION_SELECTED_FEEDRATE, s_selectedExtrusionFeedRateIndex);
	}

	void ExtrudeView::onShow() {}
	void ExtrudeView::onHide() {}

	void ExtrudeView::setToolCount(const size_t count)
	{
		Lock lock;
		if (count == getToolCount())
		{
			return;
		}
		if (count < getToolCount())
		{
			m_toolItems.resize(count);
			return;
		}

		m_toolItems.reserve(count);
		for (size_t i = getToolCount(); i < count; ++i)
		{
			m_toolItems.emplace_back(std::make_unique<ExtruderItem>(i, m_listCont, layout_t(0, 0, 100, 20)));
		}
	}

	std::shared_ptr<ExtruderItem> ExtrudeView::getExtruderItem(size_t index) const
	{
		if (index < m_toolItems.size())
		{
			return m_toolItems[index];
		}
		return nullptr;
	}
} // namespace UI
