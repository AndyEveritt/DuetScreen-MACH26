/*
 * ObjectCancelPresenter.h
 *
 *  Created on: 2025-03-18
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class ObjectCancelView;

	class ObjectCancelPresenter : public Presenter<ObjectCancelView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ObjectCancelPresenter, ObjectCancelView)

		// Actions
		void setJobObjectActive(size_t index, bool active);
		void cancelCurrentObject();
		void onObjectSelected(size_t index);

		// Observers
		void newJobBuild();
		void newJobCurrentObject();
		void newJobObjectData();
		void newAxesData();

	  private:
		void onActivate() override;
		void onInit() override
		{
			registerEventListener<EventType::JobBuild>(this, &ObjectCancelPresenter::newJobBuild);
			registerEventListener<EventType::JobCurrentObject>(this, &ObjectCancelPresenter::newJobCurrentObject);
			registerEventListener<EventType::JobObjectData>(this, &ObjectCancelPresenter::newJobObjectData);
			registerEventListener<EventType::AxesData>(this, &ObjectCancelPresenter::newAxesData);
		}

		void render();
		void updateAxisRange();
	};
} // namespace UI
