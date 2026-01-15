#include "FanView.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Home/HomeView.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"

namespace UI
{
	FanView::FanView(const std::string& name, LvObj& parent)
		: View(name, parent, layout_t(0, 0, 100, 100))
	{
		UI_LOCK();

		addStyle(Themes::getLvglStyles().bg_dark);
		addStyle(Themes::getLvglStyles().card);

		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

		m_fans.setTitle(_("fan.title"));
		m_fans.setSize(LV_PCT(100), LV_SIZE_CONTENT);
	}

	void FanView::setFanCount(size_t count)
	{
		m_fans.setItemCount(count, *this);
	}

	void FanView::setFanLabel(size_t index, std::string_view label)
	{
		UI_LOCK();
		auto fan = m_fans.getItem(index);
		if (!fan)
		{
			return;
		}
		fan->setLabel(label);
	}

	void FanView::setFanValue(size_t index, uint32_t value)
	{
		UI_LOCK();
		auto fan = m_fans.getItem(index);
		if (!fan)
		{
			return;
		}

		fan->setValue(value);
	}

	FanView::ControllableFanItem::ControllableFanItem(size_t index, LvObj& parent, FanView& view)
		: ListItem(index, parent)
		, m_view(view)
		, m_off("off", getRoot(), _("fan.off"))
		, m_slider("slider", getRoot())
		, m_max("max", getRoot(), _("fan.max"))
	{
		UI_LOCK();

		addStyle(Themes::getLvglStyles().card);

		setSize(LV_PCT(100), LV_SIZE_CONTENT);
		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_slider.setFlexGrow(1);

		m_slider.setLabel(_("fan.header"));
		m_slider.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_slider.setRange(0, 100);
		m_slider.setValueChangedCallback(
			[this](float value) { m_view.m_presenter->setFanSpeed(getIndex(), static_cast<uint32_t>(value)); });
		m_slider.setNumberPad(&HomeView::instance().getNumberPad());
		m_slider.setStylePad(5, LV_PART_MAIN, Padding::ALL);
		m_slider.addStyle(Themes::getLvglStyles().no_border, 0);

		m_off.setSize(60, LV_PCT(100));
		m_max.setSize(60, LV_PCT(100));

		m_off.setMinWidth(LV_SIZE_CONTENT);
		m_max.setMinWidth(LV_SIZE_CONTENT);

		m_off.addClickedCallback(onFanOffClicked, this);
		m_max.addClickedCallback(onFanMaxClicked, this);
	}

	void FanView::ControllableFanItem::setLabel(std::string_view label)
	{
		UI_LOCK();
		m_slider.setLabel(label);
	}

	void FanView::ControllableFanItem::setValue(uint32_t value)
	{
		UI_LOCK();
		if (m_slider.isFocused())
		{
			return;
		}
		m_slider.setValue(static_cast<float>(value));
	}

	void FanView::ControllableFanItem::onFanOffClicked(lv_event_t* e)
	{
		UI_LOCK();
		ControllableFanItem* item = static_cast<ControllableFanItem*>(lv_event_get_user_data(e));
		item->m_view.m_presenter->setFanSpeed(item->getIndex(), 0);
	}

	void FanView::ControllableFanItem::onFanMaxClicked(lv_event_t* e)
	{
		UI_LOCK();
		ControllableFanItem* item = static_cast<ControllableFanItem*>(lv_event_get_user_data(e));
		item->m_view.m_presenter->setFanSpeed(item->getIndex(), 100);
	}
} // namespace UI
