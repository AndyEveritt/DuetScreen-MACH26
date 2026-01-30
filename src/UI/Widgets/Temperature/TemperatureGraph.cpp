/*
 * TemperatureGraph.cpp
 *
 *  Created on: 2025-10-02
 *      Author: Andy Everitt
 */

#include "TemperatureGraph.h"
#include "Debug.h"

namespace UI
{
	TemperatureGraph::TemperatureGraph(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		ZoneScoped;
	}
} // namespace UI
