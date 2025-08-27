#pragma once

#include "View.h"

namespace UI
{
	void back();
	void home();

	void addHomeScreen(LvObj* view);
	void removeHomeScreen(LvObj* view, bool close = true);
	void clearHomeScreens();

	LvObj* getCurrentScreen();
	void openScreen(LvObj* view, bool closePrevious = true);
	bool closeLastScreen();
	bool closeScreen(LvObj* view, bool returnable = true);

	void openModal(LvObj* view);
	void closeAllModals();
	bool closeModal(LvObj* view);
	bool closeLastModal();

	bool close(LvObj* view);
} // namespace UI
