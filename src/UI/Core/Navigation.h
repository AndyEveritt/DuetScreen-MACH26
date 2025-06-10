#pragma once

#include "View.h"

namespace UI
{
	typedef LvObj* ViewListItem_t;
	typedef std::vector<ViewListItem_t> ViewList_t;

	void back();
	void home();

	void addHomeScreen(LvObj* view);
	void clearHomeScreens();

	LvObj* getCurrentScreen();
	void openScreen(LvObj* view, bool closePrevious = true);
	void closeLastScreen();
	void closeScreen(LvObj* view, bool returnable = true);
} // namespace UI
