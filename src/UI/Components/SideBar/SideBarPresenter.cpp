/*
 * SideBarPresenter.cpp
 *
 *  Created on: 2025-02-21
 *      Author: Andy Everitt
 */

#include "SideBarPresenter.h"
#include "Debug.h"
#include <thread>

namespace UI
{
	void SideBarPresenter::eStop()
	{
		LOG_WARN("EStop Pressed!");
		Comm::DUET.SendGcode("M112 ;"
							 "\xF0"
							 "\x0F");
		LOG_WARN("Emergency Stop sent to Duet");
		Comm::DUET.SendGcode("M999");
		LOG_WARN("Restart sent to Duet");
	}

} // namespace UI
