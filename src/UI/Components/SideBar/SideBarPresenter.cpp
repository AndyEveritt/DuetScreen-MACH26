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
		warn("EStop Pressed!");
		Comm::DUET.SendGcode("M112 ;"
							 "\xF0"
							 "\x0F");
		warn("Emergency Stop sent to Duet");
		std::this_thread::sleep_for(std::chrono::seconds(1));
		Comm::DUET.SendGcode("M999");
		warn("Restart sent to Duet");
	}

} // namespace UI
