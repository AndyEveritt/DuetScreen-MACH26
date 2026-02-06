#include "HomeView.h"

#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Settings/SettingsView.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"
#include "utils/utils.h"

namespace UI
{
	HomeView* HomeView::s_overrideInstance = nullptr;
	std::atomic<bool> HomeView::s_instanceInitialized = false;

	static constexpr int32_t s_layoutColDsc[3] = {LV_GRID_FR(1), LV_GRID_FR(9), LV_GRID_TEMPLATE_LAST};
	static constexpr int32_t s_layoutRowDsc[3] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

	HomeView::HomeView()
		: View("HomeView")
	{
		ZoneScoped;
		UI_LOCK();
		LOG_INFO("Creating UI");

		lv_obj_set_style_pad_all(lv_screen_active(), 0, LV_PART_MAIN);

		addStyle(Themes::getLvglStyles().bg_dark);
		addStyle(Themes::getLvglStyles().pad_zero);
		m_files.addStyle(Themes::getLvglStyles().bg_dark);
		m_macroView.addStyle(Themes::getLvglStyles().card);
		m_jobView.addStyle(Themes::getLvglStyles().card);

		addHomeScreen(&m_dashboard);

		setLayoutStyle(LV_LAYOUT_GRID);
		setGridDsc(s_layoutColDsc, s_layoutRowDsc);
		setGridCell(m_statusBar, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 0, 1);
		setGridCell(m_sideBar, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 2);
		setGridCell(m_mainWindow, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

		m_statusBar.setHeight(LV_SIZE_CONTENT);
		m_mainWindow.setFlexGrow(1);
		m_mainWindow.setHeight(LV_PCT(100));
		// m_mainWindow.setExtDrawSize(100); /* required for tab view outer buttons */

		m_files.setSize(LV_PCT(100), LV_PCT(100));

		// Main Window Layout
		m_sideBar.moveToFront();
		m_consoleView.hide();
		m_controlView.hide();
		m_files.hide();
		m_settingsView.hide();
#if SIDE_BAR_APP_DRAWER
		m_moveView.hide();
		m_temperatureView.hide();
		m_fanView.hide();
		m_fineTuneView.hide();
		m_heightmapView.hide();
#endif

		m_macroView.getPresenter()->setBaseFolder(FilePresenter::BaseFolder::MACROS);
		m_jobView.getPresenter()->setBaseFolder(FilePresenter::BaseFolder::GCODES);

		// Message Box
		m_alert.setMaxHeight(LV_PCT(70));

		// Update Prompt
		m_updatePrompt.setTitle(_("message.update_available"));
		m_updatePrompt.setText(_("message.update_available_text"));
		m_updatePrompt.setOkBtnText(_("message.update_confirm"));
		m_updatePrompt.setCancelBtnText(_("message.update_cancel"));
		m_updatePrompt.setOkCallback([this]() { m_presenter->update(); });
		m_updatePrompt.okVisible(true);
		m_updatePrompt.cancelVisible(true);
		m_updatePrompt.setMaxHeight(LV_PCT(70));

		// Extrusion Factor Modal
		m_extrusionFactorModal.setSize(LV_PCT(80), LV_PCT(70));

		// Keyboard
		showKeyboard(false);
		m_kb.setFlag(LV_OBJ_FLAG_FLOATING, true);
		m_kb.setAlign(LV_ALIGN_BOTTOM_MID, 0, 0);
		m_kb.setSize(LV_PCT(100), LV_PCT(50));
		m_kb.addEventCallback(
			[this](lv_event_t*)
			{
				m_kb.hide();
				if (auto ta = m_kb.getTextarea())
					ta->sendEvent(LV_EVENT_DEFOCUSED);
			},
			LV_EVENT_CANCEL);
		m_settingsView.setKeyboard(&m_kb);

		/* NumberPad */
		m_dashboard.setNumberPad(&m_numberpad);
	}

	HomeView::~HomeView()
	{
		ZoneScoped;
		removeHomeScreen(this, false);
	}

	HomeView& HomeView::instance()
	{
		ZoneScoped;
		// Allow tests to override the singleton instance when needed
		if (s_overrideInstance)
		{
			return *s_overrideInstance;
		}
		if (!s_instanceInitialized)
		{
			LOG_INFO("Initialising HomeView");
			s_instanceInitialized = true;
		}
		static HomeView view;
		return view;
	}

	void HomeView::setInstance(HomeView* instance)
	{
		ZoneScoped;
		s_overrideInstance = instance;
	}

	void HomeView::clear()
	{
		ZoneScoped;
		m_dashboard.clear();
		clearMessageBoxes();
	}

	void HomeView::onShow()
	{
		ZoneScoped;
	}

	void HomeView::onHide()
	{
		ZoneScoped;
	}

	std::shared_ptr<MessageBox>& HomeView::createMessageBox()
	{
		ZoneScoped;
		UI_LOCK();
		m_messageBoxList.emplace_back(
			std::make_shared<MessageBox>("home_message_box", getRoot(), layout_t(0, 0, 70, LV_SIZE_CONTENT)));
		auto& msgBox = m_messageBoxList.back();
		msgBox->setFlag(LV_OBJ_FLAG_FLOATING, true);
		msgBox->setAlign(LV_ALIGN_TOP_MID, 0, 2);
		msgBox->setMaxHeight(LV_PCT(70));
		msgBox->hide();
		return msgBox;
	}

	std::shared_ptr<MessageBox> HomeView::getMessageBox(size_t index) const
	{
		ZoneScoped;
		UI_LOCK();
		if (index >= getMessageBoxCount())
		{
			return nullptr;
		}
		auto it = m_messageBoxList.cbegin();
		std::advance(it, index);
		return *it;
	}

	void HomeView::popMessageBox()
	{
		ZoneScoped;
		UI_LOCK();
		if (!m_messageBoxList.empty())
		{
			m_messageBoxList.erase(m_messageBoxList.begin());
		}
	}

	void HomeView::showKeyboard(bool show)
	{
		ZoneScoped;
		UI_LOCK();
		m_alert.setAlign(LV_ALIGN_TOP_MID, 0, show ? 5 : 0);
		m_alert.setMaxHeight(show ? LV_PCT(45) : LV_PCT(70));
		m_kb.setVisible(show, true);
	}

	void HomeView::showUpdatePrompt(bool show)
	{
		ZoneScoped;
		UI_LOCK();
		m_updatePrompt.setVisible(show);
	}
} // namespace UI
