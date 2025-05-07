/*
 * SideBarPresenter.cpp
 *
 *  Created on: 2025-02-21
 *      Author: Andy Everitt
 */

#include "SideBarPresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include <thread>

namespace UI
{
	void SideBarPresenter::eStop()
	{
		LOG_WARN("EStop Pressed!");
		Comm::DUET.SendGcode("M112 M999\n");
		LOG_WARN("Emergency Stop sent to Duet");
		Comm::DUET.Disconnect();
		Model::get().post<EventType::Response>("Emergency Stop, trying to reconnect...");
	}

} // namespace UI
