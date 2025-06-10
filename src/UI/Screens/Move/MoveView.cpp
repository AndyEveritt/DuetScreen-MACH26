#include "MoveView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	static constexpr float s_relMoveValues[] = {-50, -10, -1.0f, -0.1f, 0.1f, 1.0f, 10.0f, 50.0f}; // mm
	static constexpr uint32_t s_feedRates[] = {300, 100, 50, 20, 10, 5};						   // mm/s
	static uint32_t s_currentFeedRateIndex = 2;

	AxisItem::AxisItem(const size_t index, lv_obj_t* parent, MoveView& list)
		: ListItem("move_axis_item", index, parent)
		, m_list(list)
		, m_home(utils::format("move_axis_%u_home", index), getCont(), "", layout_t(0, 0, 0, 100))
		, m_relMove(utils::format("move_axis_%u_rel_move", index), getCont())
		, m_toolPosition(lv_label_create(getCont()))
		, m_machinePosition(lv_label_create(getCont()))
	{
		// Layout
		UI_LOCK();
		constexpr lv_coord_t pad = 2;
		lv_obj_set_size(getCont(), LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_style_pad_all(getCont(), pad, 0);
		lv_obj_set_style_pad_column(getCont(), pad, 0);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_home.getCont(), 4);

		m_relMove.setListFlow(LV_FLEX_FLOW_ROW);
		m_relMove.setPad(0);
		m_relMove.setListPad(0);
		m_relMove.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_relMove.setListSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_relMove.setItemCount(ARRAY_SIZE(s_relMoveValues),
							   [&index, this](size_t i, lv_obj_t* parent)
							   {
								   auto btn =
									   std::make_shared<Button>(utils::format("axis_%u_move_rel_button_%u", index, i),
																parent,
																utils::format("%.1f", s_relMoveValues[i]).c_str());
								   btn->setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(i)));
								   btn->setCallback(onRelMoveEvent, LV_EVENT_CLICKED, this);
								   btn->setFlexGrow(1);
								   btn->setHeight(LV_SIZE_CONTENT);
								   btn->addStyle(Themes::getLvglStyles().actionBtn, 0);
								   return btn;
							   });

		lv_obj_set_flex_grow(m_toolPosition, 5);
		lv_obj_set_flex_grow(m_machinePosition, 5);
		lv_obj_set_style_text_align(m_toolPosition, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_set_style_text_align(m_machinePosition, LV_TEXT_ALIGN_CENTER, 0);

		lv_label_set_text(m_toolPosition, "");
		lv_label_set_text(m_machinePosition, "");

		m_home.setCallback(onHomeEvent, LV_EVENT_CLICKED, this);
		m_home.addStyle(Themes::getLvglStyles().actionBtn, 0);

		// Styles
		m_home.setCheckable(true);
		m_home.addStyle(Themes::getComponentStyles().unhomed, LV_STATE_CHECKED);
	}

	AxisItem::~AxisItem() {}

	void AxisItem::setAxisLetter(const char* letter)
	{
		UI_LOCK();
		m_axisLetter = letter;
		m_home.setText(utils::format(_("move_axis_home"), letter).c_str());
	}

	void AxisItem::setHomed(const bool homed)
	{
		UI_LOCK();
		m_home.setChecked(!homed);
	}

	void AxisItem::setToolPosition(const float& position)
	{
		UI_LOCK();
		lv_label_set_text(m_toolPosition, utils::format("%.2f", position).c_str());
	}

	void AxisItem::setMachinePosition(const float& position)
	{
		UI_LOCK();
		lv_label_set_text(m_machinePosition, utils::format("(%.2f)", position).c_str());
	}

	void AxisItem::disableHome(const bool disabled)
	{
		UI_LOCK();
		m_home.setInvalid(disabled);
		m_home.setText(disabled ? m_axisLetter.c_str()
								: utils::format(_("move_axis_home"), m_axisLetter.c_str()).c_str());
	}

	void AxisItem::onHomeEvent(lv_event_t* e)
	{
		UI_LOCK();
		AxisItem* item = static_cast<AxisItem*>(lv_event_get_user_data(e));
		item->getList().m_presenter->homeAxis(item->getIndex());
	}

	void AxisItem::onRelMoveEvent(lv_event_t* e)
	{
		UI_LOCK();
		AxisItem* item = static_cast<AxisItem*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
		uintptr_t index = (uintptr_t)lv_obj_get_user_data(btn);

		float distance = s_relMoveValues[index];
		item->getList().m_presenter->moveAxisRelative(item->getIndex(), distance, s_feedRates[s_currentFeedRateIndex]);
	}

	MoveView::MoveView(lv_obj_t* parent)
		: View(lv_obj_create, "move_view", parent, layout_t(0, 0, 100, 100))
		, m_layoutColDsc{LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_layoutRowDsc{LV_GRID_FR(1), 30, LV_GRID_FR(3), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST}
		, m_topBarCont(lv_obj_create(getCont()))
		, m_listHeader(lv_obj_create(getCont()))
		, m_bottomBarCont(lv_obj_create(getCont()))
		, m_homeAll("move_home_all", m_topBarCont, _("home_all"), layout_t(0, 0, 0, 100))
		, m_trueBedLevel("move_true_bed_level", m_topBarCont, _("true_bed_level"), layout_t(0, 0, 0, 100))
		, m_meshBedLevel("move_mesh_bed_level", m_topBarCont, _("mesh_bed_level"), layout_t(0, 0, 0, 100))
		, m_heightmap("move_heightmap", m_topBarCont, _("heightmap"), layout_t(0, 0, 0, 100))
		, m_disableMotors("move_disable_motors", m_topBarCont, _("disable_motors"), layout_t(0, 0, 0, 100))
		, m_listHeaderPadding(lv_obj_create(m_listHeader))
		, m_toolPositionLabel(lv_label_create(m_listHeader))
		, m_machinePositionLabel(lv_label_create(m_listHeader))
		, m_axisItems("move_list", getCont())
		, m_feedRates("move_feed_rates", m_bottomBarCont)
	{
		UI_LOCK();

		m_homeAll.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_trueBedLevel.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_meshBedLevel.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_disableMotors.addStyle(Themes::getLvglStyles().actionBtn, 0);

		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(getCont(), m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(m_topBarCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_listHeader, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_set_grid_cell(m_axisItems, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
		lv_obj_set_grid_cell(m_bottomBarCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 3, 1);

		// Top Bar
		constexpr lv_coord_t pad = 10;
		lv_obj_set_style_pad_all(m_topBarCont, pad, 0);
		lv_obj_set_style_pad_column(m_topBarCont, pad, 0);
		lv_obj_set_flex_flow(m_topBarCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_topBarCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_homeAll.getCont(), 1);
		lv_obj_set_flex_grow(m_trueBedLevel.getCont(), 1);
		lv_obj_set_flex_grow(m_meshBedLevel.getCont(), 1);
		lv_obj_set_flex_grow(m_heightmap.getCont(), 1);
		lv_obj_set_flex_grow(m_disableMotors.getCont(), 1);

		// List Header
		lv_obj_set_flex_flow(m_listHeader, LV_FLEX_FLOW_ROW);
		lv_obj_set_style_pad_ver(m_listHeader, 0, 0);
		lv_obj_set_style_pad_column(m_listHeader, pad, 0);
		lv_obj_set_flex_grow(m_listHeaderPadding, 10);
		lv_obj_set_height(m_listHeaderPadding, LV_PCT(0));
		lv_obj_set_flex_grow(m_toolPositionLabel, 5);
		lv_obj_set_flex_grow(m_machinePositionLabel, 5);
		lv_obj_set_height(m_toolPositionLabel, LV_PCT(100));
		lv_obj_set_height(m_machinePositionLabel, LV_PCT(100));
		lv_obj_set_style_text_align(m_toolPositionLabel, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_set_style_text_align(m_machinePositionLabel, LV_TEXT_ALIGN_CENTER, 0);
		lv_label_set_text(m_toolPositionLabel, _("move_tool_position"));
		lv_label_set_text(m_machinePositionLabel, _("move_machine_position"));

		m_homeAll.setCallback(onHomeAllEvent, LV_EVENT_CLICKED, this);
		m_trueBedLevel.setCallback(onTrueBedLevelEvent, LV_EVENT_CLICKED, this);
		m_meshBedLevel.setCallback(onMeshBedLevelEvent, LV_EVENT_CLICKED, this);
		m_heightmap.setCallback(onHeightmapEvent, LV_EVENT_CLICKED, this);
		m_disableMotors.setCallback(onDisableMotorsEvent, LV_EVENT_CLICKED, this);

		// List
		m_axisItems.setPad(0);
		m_axisItems.setListPad(0);

		// Bottom Bar
		lv_obj_set_style_pad_all(m_bottomBarCont, pad, 0);
		lv_obj_set_style_pad_column(m_bottomBarCont, pad, 0);
		lv_obj_set_flex_flow(m_bottomBarCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_bottomBarCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		assert(s_currentFeedRateIndex < ARRAY_SIZE(s_feedRates));

		m_feedRates.setTitle(_("move_feedrate"));
		m_feedRates.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_feedRates.setListSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_feedRates.setListFlow(LV_FLEX_FLOW_ROW);
		m_feedRates.setListPad(0);
		m_feedRates.setItemCount(ARRAY_SIZE(s_feedRates),
								 [this](size_t i, lv_obj_t* parent)
								 {
									 auto btn = std::make_shared<Button>(utils::format("move_feed_rate_%u", i),
																		 parent,
																		 utils::format("%u", s_feedRates[i]).c_str());
									 btn->setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(i)));
									 btn->setCallback(onFeedRateEvent, LV_EVENT_CLICKED, this);
									 btn->setCheckable(true);
									 btn->setFlexGrow(1);
									 lv_obj_set_height(btn->getCont(), LV_SIZE_CONTENT);
									 return btn;
								 });
		m_feedRates.getItem(s_currentFeedRateIndex)->setChecked(true);
	}

	void MoveView::onHomeAllEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		view->m_presenter->homeAll();
	}

	void MoveView::onTrueBedLevelEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		view->m_presenter->trueBedLevel();
	}

	void MoveView::onMeshBedLevelEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		view->m_presenter->meshBedLevel();
	}

	void MoveView::onHeightmapEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		view->m_presenter->heightmap();
	}

	void MoveView::onDisableMotorsEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		view->m_presenter->disableMotors();
	}

	void MoveView::onFeedRateEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
		view->m_feedRates.getItem(s_currentFeedRateIndex)->setChecked(false);
		s_currentFeedRateIndex = reinterpret_cast<uintptr_t>(lv_obj_get_user_data(btn));
		view->m_feedRates.getItem(s_currentFeedRateIndex)->setChecked(true);
	}

	void MoveView::onShow() {}
	void MoveView::onHide() {}

	void MoveView::setAxisCount(const size_t count)
	{
		m_axisItems.setItemCount(count, *this);
	}

	std::shared_ptr<AxisItem> MoveView::getAxisItem(size_t index) const
	{
		return m_axisItems.getItem(index);
	}
} // namespace UI
