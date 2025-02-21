/*
 * {{file_name_with_ext}}
 *
 *  Created on: {{date}}
 *      Author: {{author}}
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class MyView;

	class {{file_name}} : public Presenter<MyView>
	{
	  public:
  		using Presenter::Presenter;

	  protected:
	};
} // namespace UI
