/*
 * TemperatureGraph.h
 *
 *  Created on: 2025-10-02
 *      Author: Andy Everitt
 */

#pragma once

#include "TemperatureGraphPresenter.h"
#include "UI/Components/Graph/Graph.h"
#include "UI/Core/View.h"

namespace UI
{
	class TemperatureGraph : public View<TemperatureGraphPresenter, Graph>
	{
	  public:
		TemperatureGraph(const std::string& name, LvObj& parent);

	  private:
	};
} // namespace UI
