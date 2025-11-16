/*
 * FilamentSelectDropdown.h
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#pragma once

#include "FilamentSelectDropdownPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/DropdownMenu.h"
#include "UI/Components/List/List.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Components/Modal/Modal.h"
#include "UI/Core/View.h"

namespace UI
{
	class FilamentSelectDropdown : public View<FilamentSelectDropdownPresenter>
	{
	  public:
		FilamentSelectDropdown(const std::string& name, LvObj& parent);

		void setToolIndex(size_t index);

		void showHint(bool show);

		void setFilamentOptions(const std::vector<std::string>& options);
		void setFilamentSelected(const std::string& filament);
		void setFilamentDisabled(bool disabled);

	  private:
		static void onFilamentSelectEvent(lv_event_t* e);
		static void onFilamentChangeEvent(lv_event_t* e);
		static void onFilamentUnloadEvent(lv_event_t* e);

		DropdownMenu m_selection{"filament_select", getRoot()};
		Button m_changeBtn{"filament_change", getRoot()};
		Button m_unloadBtn{"filament_load_unload", getRoot()};

		std::vector<std::string> m_filamentOptions;
		std::string m_loadedFilament;
	};
} // namespace UI
