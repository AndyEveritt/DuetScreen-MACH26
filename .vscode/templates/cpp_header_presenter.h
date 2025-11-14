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

		// Setters

		// Getters

		// Actions

		// Observers

	  protected:
		virtual void onInit() override {}
		virtual void onActivate() override {}
		virtual void onDeactivate() override {}

		virtual void onConnect() override {}
		virtual void onDisconnect() override {}
	};
} // namespace UI
