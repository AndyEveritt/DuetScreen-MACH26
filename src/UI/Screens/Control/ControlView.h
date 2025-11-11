#pragma once

#include "ControlPresenter.h"
#include "UI/Components/Containers/TabView.h"
#include "UI/Screens/Move/MoveView.h"
#include "UI/Screens/Temperature/TemperatureView.h"
#include "UI/Screens/Heightmap/HeightmapView.h"
#include "UI/Screens/Fan/FanView.h"
#include "UI/Core/View.h"
#include "i18n/i18n.h"

namespace UI
{
	class ControlView : public View<ControlPresenter>
	{
	  public:
		ControlView(const std::string& name, LvObj& parent);

	  private:
		TabView m_tabs{"tabs", getRoot()};
		MoveView m_moveView{"move", m_tabs.addTab(_("control.move_tab"))};
		TemperatureView m_temperatureView{"temperature", m_tabs.addTab(_("control.temperature_tab"))};
		HeightmapView m_heightmapView{"heightmap", m_tabs.addTab(_("control.heightmap_tab"))};
		FanView m_fanView{"fan", m_tabs.addTab(_("control.fan_tab"))};
	};
} // namespace UI