/*
 * FineTunePresenter.cpp
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#include "FineTunePresenter.h"
#include "Debug.h"
#include "FineTune.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/Tool.h"

namespace UI
{
	static float s_babyStepValues[] = {0.01f, 0.05f};
	static float s_speedValues[] = {5.0f, 25.0f};
	static float s_flowValues[] = {1.0f, 2.0f};

	void FineTunePresenter::newSpeedFactor()
	{
		ModelLock lock;
		m_view->setSpeedValue(OM::Move::GetSpeedFactor());
	}

	void FineTunePresenter::newExtruderData()
	{
		ModelLock lock;
		OM::Tool* tool = OM::GetCurrentTool();
		if (tool == nullptr)
		{
			m_view->setFlowValue(100);
			return;
		}
		// TODO show all extruder multipliers
		size_t extruderCount = 0;
		uint32_t flowMultiplier = 0;
		tool->IterateExtruders(
			[&](OM::Move::ExtruderAxis* extruder, size_t index)
			{
				flowMultiplier += 100 * extruder->factor;
				extruderCount++;
			});

		if (extruderCount == 0)
		{
			m_view->setFlowValue(100);
			return;
		}

		m_view->setFlowValue(flowMultiplier / extruderCount);
	}

} // namespace UI
