/*
 * EStopPresenter.cpp
 *
 *  Created on: 2025-08-27
 *      Author: Andy Everitt
 */

#include "EStopPresenter.h"
#include "Debug.h"
#include "EStop.h"
#include "Hardware/Duet.h"
#include "i18n/i18n.h"

namespace UI
{
	void EStopPresenter::eStop()
	{
		LOG_WARN("EStop Pressed!");
		Comm::DUET.SendGcode("M112 M999\n");
		LOG_WARN("Emergency Stop sent to Duet");
		Comm::DUET.Disconnect();
		Model::get().post<EventType::Response>(ResponseType::WARNING, _("estop.message"));
	}

} // namespace UI
