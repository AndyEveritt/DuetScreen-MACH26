#pragma once

#include "HomePresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/Graph/Graph.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Components/NumberPad/NumberPad.h"
#include "UI/Components/SideBar/SideBar.h"
#include "UI/Components/ToolList/ToolList.h"
#include "UI/Core/View.h"
#include "UI/Screens/Console/ConsoleView.h"
#include "UI/Screens/Extrude/ExtrudeView.h"
#include "UI/Screens/Fan/FanView.h"
#include "UI/Screens/File/FileView.h"
#include "UI/Screens/Move/MoveView.h"
#include "UI/Screens/Settings/SettingsView.h"
#include "UI/Screens/Status/StatusView.h"
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

		virtual void refresh() override;

		std::shared_ptr<MessageBox> createMessageBox();
		size_t getMessageBoxCount() const { return m_messageBoxList.size(); }
		std::shared_ptr<MessageBox> getMessageBox(size_t index) const;
		void popMessageBox();
		void clearMessageBoxes() { m_messageBoxList.clear(); }

		// Keyboard
		void showKeyboard(bool show);
		lv_obj_t* getKeyboard() const { return m_kb; }

		// Sub-views
		ConsoleView& getConsoleView() { return m_consoleView; }
		MoveView& getMoveView() { return m_moveView; }
		ExtrudeView& getExtrudeView() { return m_extrudeView; }
		FileView& getFileView() { return m_fileView; }
		SettingsView& getSettingsView() { return m_settingsView; }
		StatusView& getStatusView() { return m_statusView; }

	  private:
		HomeView();
		virtual void onShow() {}
		virtual void onHide();

		static void onWindowSelectEvent(lv_event_t* e);

		SideBar m_sideBar;
		lv_obj_t* m_mainWindow;
		ToolList m_toolList;
		Graph m_graph;

		// Window selector
		lv_obj_t* m_windowSelect;
		Button m_moveWindow;
		Button m_extrudeWindow;
		Button m_statusWindow;
		Button m_heightmapWindow;
		Button m_fansWindow;
		Button m_objectCancelWindow;
		Button m_filesWindow;
		Button m_settingsWindow;

		// Windows
		ConsoleView m_consoleView;
		MoveView m_moveView;
		ExtrudeView m_extrudeView;
		FanView m_fanView;
		FileView m_fileView;
		SettingsView m_settingsView;
		StatusView m_statusView;

		// Message box
		std::list<std::shared_ptr<MessageBox>> m_messageBoxList;
		MessageBox m_alert;

		lv_obj_t* m_kb;
	};
} // namespace UI