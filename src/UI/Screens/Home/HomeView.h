#pragma once

#include "HomePresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Graph/Graph.h"
#include "UI/Components/Input/ModalNumberPad.h"
#include "UI/Components/Input/NumberPad.h"
#include "UI/Components/LVGL/LvKeyboard.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Core/View.h"
#include "UI/Screens/Console/ConsoleView.h"
#include "UI/Screens/Fan/FanView.h"
#include "UI/Screens/File/FileView.h"
#include "UI/Screens/Heightmap/HeightmapView.h"
#include "UI/Screens/Move/MoveView.h"
#include "UI/Screens/Settings/SettingsView.h"
#include "UI/Screens/Status/StatusView.h"
#include "UI/Screens/Temperature/TemperatureView.h"
#include "UI/Widgets/SideBar/SideBar.h"
#include "UI/Widgets/StatusBar/StatusBar.h"
#include "UI/Widgets/ToolList/ToolList.h"
#include "lvgl/lvgl.h"
#include <list>
#include <memory>

namespace UI
{
	class HomeView : public View<HomePresenter>
	{
	  public:
		friend class HomePresenter;

		static HomeView& instance()
		{
			static HomeView view;
			return view;
		}

		void clear();

		std::shared_ptr<MessageBox> createMessageBox();
		size_t getMessageBoxCount() const { return m_messageBoxList.size(); }
		std::shared_ptr<MessageBox> getMessageBox(size_t index) const;
		void popMessageBox();
		void clearMessageBoxes() { m_messageBoxList.clear(); }

		// Keyboard
		void showKeyboard(bool show);
		LvKeyboard& getKeyboard() { return m_kb; }

		// Sub-views
		ConsoleView& getConsoleView() { return m_consoleView; }
		MoveView& getMoveView() { return m_moveView; }
		TemperatureView& getExtrudeView() { return m_temperatureView; }
		FileView& getFileView() { return m_fileView; }
		SettingsView& getSettingsView() { return m_settingsView; }
		StatusView& getStatusView() { return m_statusView; }

		void showUpdatePrompt(bool show);

	  private:
		HomeView();
		virtual void onShow();
		virtual void onHide();

		static void onWindowSelectEvent(lv_event_t* e);

		StatusBar m_statusBar;
		SideBar m_sideBar;
		lv_obj_t* m_mainWindow;
		ToolList m_toolList;
		Graph m_graph;

		// Window selector
		lv_obj_t* m_windowSelect;
		Button m_moveWindow;
		Button m_temperatureWindow;
		Button m_statusWindow;
		Button m_heightmapWindow;
		Button m_fansWindow;
		Button m_objectCancelWindow;
		Button m_filesWindow;
		Button m_settingsWindow;

		// Windows
		ConsoleView m_consoleView;
		MoveView m_moveView;
		TemperatureView m_temperatureView;
		FanView m_fanView;
		FileView m_fileView;
		HeightmapView m_heightmapView;
		SettingsView m_settingsView;
		StatusView m_statusView;

		// Message box
		std::list<std::shared_ptr<MessageBox>> m_messageBoxList;
		MessageBox m_alert;

		// Update prompt
		MessageBox m_updatePrompt;

		ModalNumberPad m_numberpad;

		LvKeyboard m_kb;
	};
} // namespace UI