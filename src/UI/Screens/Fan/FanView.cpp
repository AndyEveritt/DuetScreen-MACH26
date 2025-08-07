#include "FanView.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	FanView::FanView(lv_obj_t* parent)
		: View("fan_view", parent, layout_t(0, 0, 100, 100))
		, m_fanHeader(lv_label_create(getRoot()))
	{
		UI_LOCK();

		addStyle(Themes::getLvglStyles().bg_dark);

		lv_obj_set_layout(getRoot(), LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(getRoot(), LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(getRoot(), LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

		lv_label_set_text(m_fanHeader, _("fan_header"));
	}

	void FanView::setFanCount(size_t count)
	{
		UI_LOCK();
		if (count == getFanCount())
		{
			return;
		}

		if (count < getFanCount())
		{
			m_fans.resize(count);
			return;
		}

		m_fans.reserve(count);
		for (size_t i = getFanCount(); i < count; ++i)
		{
			m_fans.emplace_back(std::make_shared<FanItem>(i, *this));
		}
	}

	void FanView::setFanLabel(size_t index, const char* label)
	{
		UI_LOCK();
		if (index >= m_fans.size())
		{
			return;
		}
		m_fans[index]->setLabel(label);
	}

	void FanView::setFanValue(size_t index, uint32_t value)
	{
		UI_LOCK();
		if (index >= m_fans.size())
		{
			return;
		}

		m_fans[index]->setValue(value);
	}

	FanView::FanItem::FanItem(size_t index, FanView& view)
		: LvObj(lv_obj_create,
				utils::format("fan_item_%d", index).c_str(),
				view.getRoot(),
				layout_t(0, 0, 100, LV_SIZE_CONTENT))
		, m_view(view)
		, m_index(index)
		, m_off(utils::format("fan_off_%d", index).c_str(), getRoot(), _("off"))
		, m_slider(utils::format("fan_slider_%d", index).c_str(), getRoot())
		, m_max(utils::format("fan_max_%d", index).c_str(), getRoot(), _("max"))
	{
		UI_LOCK();
		lv_obj_set_layout(getRoot(), LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(getRoot(), LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getRoot(), LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_slider.getRoot(), 1);

		m_slider.setLabel(_("fan"));
		m_slider.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_slider.setRange(0, 100);
		m_slider.setValueChangedCallback([this](int32_t value) { m_view.m_presenter->setFanSpeed(m_index, value); });
		m_slider.setStylePad(5, LV_PART_MAIN, Padding::ALL);
		m_slider.addStyle(Themes::getLvglStyles().no_border, 0);

		m_off.addClickedCallback(onFanOffClicked, this);
		m_max.addClickedCallback(onFanMaxClicked, this);
	}

	void FanView::FanItem::setLabel(const char* label)
	{
		UI_LOCK();
		m_slider.setLabel(label);
	}

	void FanView::FanItem::setValue(uint32_t value)
	{
		UI_LOCK();
		if (m_slider.isFocused())
		{
			return;
		}
		m_slider.setValue(value);
	}

	void FanView::FanItem::onFanOffClicked(lv_event_t* e)
	{
		UI_LOCK();
		FanItem* item = static_cast<FanItem*>(lv_event_get_user_data(e));
		item->m_view.m_presenter->setFanSpeed(item->m_index, 0);
	}

	void FanView::FanItem::onFanMaxClicked(lv_event_t* e)
	{
		UI_LOCK();
		FanItem* item = static_cast<FanItem*>(lv_event_get_user_data(e));
		item->m_view.m_presenter->setFanSpeed(item->m_index, 100);
	}
} // namespace UI
