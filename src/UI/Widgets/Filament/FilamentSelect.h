/*
 * FilamentSelect.h
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#pragma once

#include "FilamentSelectPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/List/List.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Core/View.h"

namespace UI
{
	class FilamentSelect : public View<FilamentSelectPresenter>
	{
	  public:
		class ToolItem;

		FilamentSelect(const std::string& name, lv_obj_t* parent);

		void setToolCount(size_t count);
		void setToolData(size_t index, std::string_view toolName, std::string_view filamentName);
		void setFilamentOptions(const std::vector<std::string>& options);

		void setMessageBox(MessageBox* confirmation) { m_confirmation = confirmation; }
		MessageBox* getMessageBox() const { return m_confirmation; }

		void showToolSelect(bool show);
		void setSelectedFilament(std::string_view filamentName);

	  private:
		static void onFilamentOptionClicked(lv_event_t* e);

		void onShow() override;

		LvLabel m_header;
		LvContainer m_cont;
		List<ToolItem> m_toolList;
		List<Button> m_filamentOptions;

		MessageBox* m_confirmation = nullptr;
	};
} // namespace UI
