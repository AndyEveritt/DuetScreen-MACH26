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

	  private:
        LvLabel m_header;
		List<ToolItem> m_toolList;
		List<Button> m_filamentOptions;
	};
} // namespace UI
