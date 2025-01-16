#pragma once

#include "view.h"

namespace UI
{
	typedef BaseView* ViewListItem_t;
	typedef std::vector<ViewListItem_t> ViewList_t;

	void back();
	void home();

	void addHomeScreen(BaseView* view);
	void clearHomeScreens();

	BaseView* getCurrentScreen();
	void openScreen(BaseView* view, bool closePrevious = true);
	void closeLastScreen();
	void closeScreen(BaseView* view, bool returnable = true);
} // namespace UI
