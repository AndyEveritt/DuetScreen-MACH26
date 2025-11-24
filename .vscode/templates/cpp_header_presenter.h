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
		void onInit() override {}
		void onActivate() override {}
		void onDeactivate() override {}

		void onConnect() override {}
		void onDisconnect() override {}
	};
} // namespace UI
