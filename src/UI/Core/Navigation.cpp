#include "Navigation.h"
#include "Debug.h"
#include <algorithm>
#include <vector>

namespace UI
{
	static ViewList_t s_homeScreens;
	static ViewList_t s_openScreens;
	static ViewList_t s_returnableScreens;

	static bool inVector(ViewList_t& vec, ViewListItem_t item)
	{
		auto it = std::find(vec.begin(), vec.end(), item);
		if (it != vec.end())
		{
			return true;
		}
		return false;
	}

	static bool removeFromVector(ViewList_t& vec, ViewListItem_t item)
	{
		auto it = std::find(vec.begin(), vec.end(), item);
		if (it != vec.end())
		{
			dbg("Removing screen '%s' from vector", item->getName());
			vec.erase(it);
			return true;
		}
		return false;
	}

	static bool addToVector(ViewList_t& vec, ViewListItem_t item)
	{
		removeFromVector(vec, item);
		dbg("Adding screen %d to vector", item->getName());
		vec.push_back(item);
		return true;
	}

	/**
	 * @brief Handles the action of the back button being pressed.
	 *
	 * This function performs the following steps:
	 *
	 * 1. Logs the back button press event.
	 *
	 * 2. Retrieves the current screen. If there is no current screen, logs a warning and returns.
	 *
	 * 3. If the current screen handles the back action, returns.
	 *
	 * 4. If there are open screens, closes the last open screen.
	 *
	 * 5. If there are returnable screens, opens the last returnable screen.
	 */
	void back()
	{
		info("Back button pressed");
		ViewListItem_t currentScreen = getCurrentScreen();
		if (currentScreen == nullptr)
		{
			warn("No current screen");
			return;
		}
		if (currentScreen->back())
		{
			return;
		}

		closeScreen(currentScreen, false);

		if (!s_returnableScreens.empty())
		{
			ViewListItem_t lastReturnable = s_returnableScreens.back();
			openScreen(lastReturnable);
		}
	}

	/**
	 * @brief Show all home screens and hide all other screens.
	 *
	 * Clears the open screens and returnable screens lists.
	 */
	void home()
	{
		info("Home button pressed");
		for (auto screen : s_openScreens)
		{
			if (!screen->isVisible())
			{
				continue;
			}
			closeScreen(screen, false);
		}
		for (auto screen : s_homeScreens)
		{
			openScreen(screen);
		}

		s_openScreens.clear();
		s_returnableScreens.clear();
	}

	/**
	 * @brief Adds a view to the list of home screens.
	 *
	 * @param view The view to be added.
	 */
	void addHomeScreen(ViewListItem_t view)
	{
		s_homeScreens.push_back(view);
	}

	/**
	 * @brief Clears all home screens.
	 */
	void clearHomeScreens()
	{
		s_homeScreens.clear();
	}

	/**
	 * @brief Retrieves the current screen from the list of open screens.
	 *
	 * This function checks if there are any open screens in the list. If the list
	 * is empty, it logs a warning message and returns nullptr. Otherwise, it returns
	 * the last screen in the list.
	 *
	 * @return ViewListItem_t The current screen if available, otherwise nullptr.
	 */
	ViewListItem_t getCurrentScreen()
	{
		if (s_openScreens.empty())
		{
			warn("No current screen");
			return nullptr;
		}
		return s_openScreens.back();
	}

	/**
	 * @brief Opens a specified screen and optionally closes the previous screen.
	 *
	 * This function attempts to open the screen specified by the `view` parameter.
	 * If `closePrevious` is true, the last opened screen will be closed before opening the new one.
	 *
	 * @param view The screen to be opened. Must not be nullptr.
	 * @param closePrevious If true, the last opened screen will be closed before opening the new one.
	 *
	 * @note If `view` is nullptr, a warning will be logged and the function will return without opening any screen.
	 * @note If the screen is already visible, it will not be shown again.
	 * @note The screen will be removed from the list of returnable screens and added to the list of open screens if it
	 * is not a home screen.
	 */
	void openScreen(ViewListItem_t view, bool closePrevious)
	{
		if (view == nullptr)
		{
			warn("Trying to open a nullptr screen");
			return;
		}

		if (closePrevious)
		{
			closeLastScreen();
		}

		info("Opening screen '%s'", view->getName());
		view->show();
		removeFromVector(s_returnableScreens, view);
		if (!inVector(s_homeScreens, view))
		{
			addToVector(s_openScreens, view);
		}
	}

	/**
	 * @brief Closes the last visible screen.
	 *
	 * This function iterates through the list of home screens and closes any visible screen.
	 * If there are no visible home screens, it closes the last screen in the list of open screens.
	 * If the list of open screens is empty, the function returns without performing any action.
	 */
	void closeLastScreen()
	{
		info("Closing last screen");
#if 0
		for (auto screen : s_homeScreens)
		{
			if (!screen->isVisible())
			{
				continue;
			}
			closeScreen(screen);
		}
#endif
		if (s_openScreens.empty())
		{
			return;
		}
		closeScreen(s_openScreens.back());
	}

	/**
	 * @brief Closes the specified screen and updates screen vectors accordingly.
	 *
	 * This function hides the given screen if it is currently visible, removes it from the list of open screens,
	 * and optionally adds it to the list of returnable screens.
	 *
	 * @param view The screen to be closed.
	 * @param returnable If true, the screen will be added to the list of returnable screens.
	 */
	void closeScreen(ViewListItem_t view, bool returnable)
	{
		info("Closing screen '%s'", view->getName());
		if (view->isVisible())
		{
			view->hide();
		}
		removeFromVector(s_openScreens, view);
		if (returnable)
		{
			addToVector(s_returnableScreens, view);
		}
	}

} // namespace UI
