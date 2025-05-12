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
		PRESENTER_CONSTRUCTOR({{file_name}}, MyView);

	  protected:
		virtual void onInit() override {}
		virtual void onActivate() override {}
		virtual void onDeactivate() override {}
	};
} // namespace UI
