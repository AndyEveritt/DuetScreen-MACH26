/*
 * AppDrawer.cpp
 *
 *  Created on: 2025-08-05
 *      Author: Andy Everitt
 */

#include "AppDrawer.h"
#include "Debug.h"
#include "UI/Screens/Home/HomeView.h"
#include "i18n/i18n.h"

namespace UI
{
	struct AppInfo
	{
		std::string_view name;
		LvContainer* screen;
	};

	AppDrawer::AppDrawer(const std::string& name, LvObj& parent)
		: Card(name, parent)
		, m_apps("apps", getRoot())
	{
		setMinWidth(LV_SIZE_CONTENT);
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_apps.setWidth(LV_SIZE_CONTENT);
		m_apps.setFlexGrow(1);
		m_apps.setTitle(_("app_drawer.select_view"));

		auto& list_cont = m_apps.getListContainer();
		list_cont.setWidth(LV_SIZE_CONTENT);
		// list_cont.setFlag(LV_OBJ_FLAG_SCROLLABLE, true);
		list_cont.setFlexGrow(1);
		list_cont.setFlexFlow(LV_FLEX_FLOW_COLUMN_WRAP);
		list_cont.setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	}

	void AppDrawer::init()
	{
		static const AppInfo apps[] = {
			{_("app_drawer.console"), &HomeView::instance().getConsoleView()},
			{_("app_drawer.move"), &HomeView::instance().getMoveView()},
			{_("app_drawer.temperature"), &HomeView::instance().getTemperatureView()},
			// {_("app_drawer.fan"), &HomeView::instance().getFanView()},
			{_("app_drawer.fine_tune"), &HomeView::instance().getFineTuneView()},
			// {_("app_drawer.macros"), &HomeView::instance().getMacroView()},
			{_("app_drawer.heightmap"), &HomeView::instance().getHeightmapView()},
			{_("app_drawer.settings"), &HomeView::instance().getSettingsView()},
			{_("app_drawer.status"), &HomeView::instance().getDashboard().getStatusView()},
		};

		m_apps.setItemCount(std::size(apps),
							[](size_t index, LvObj& parent)
							{
								auto app = std::make_shared<App>(index, parent);
								const auto& appInfo = apps[index];
								app->setName(appInfo.name);
								app->setSize(100, 100);
								app->setMinWidth(LV_SIZE_CONTENT);
								// app->setMinHeight(LV_SIZE_CONTENT);
								// app->setFlexGrow(1);
								app->setUserData((void*)appInfo.screen);
								return app;
							});
	}
} // namespace UI
