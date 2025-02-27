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
#include "lv_i18n/lv_i18n.h"

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

		m_view->setExtruderCount(OM::Move::GetExtruderAxisCount());

		OM::Move::IterateExtruderAxesWhile(
			[this](OM::Move::ExtruderAxis* extruder, size_t index)
			{
				m_view->setExtruderLabel(index, utils::format(_("fine_tune_extruder"), extruder->index).c_str());
				m_view->setExtruderValue(index, 100 * extruder->factor);
				return true;
			});
	}

} // namespace UI
