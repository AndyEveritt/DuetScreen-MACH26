/*
 * ToolListPresenter.h
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class ToolListItem;
	class ToolList;

	class ToolListItemPresenter : public Presenter<ToolListItem>
	{
	  public:
		using Presenter::Presenter;

		void setToolIndex(int8_t index);

		void newToolData();

	  private:
		int8_t m_toolIndex = -1;
	};

	class ToolListPresenter : public Presenter<ToolList>
	{
	  public:
		using Presenter::Presenter;

		void newToolData();

	  private:
	};
} // namespace UI
