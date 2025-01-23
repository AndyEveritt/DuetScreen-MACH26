#include "ExtrudePresenter.h"
#include "Debug.h"
#include "ExtrudeView.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Tool.h"

namespace UI
{
	void ExtrudePresenter::retract(uint32_t distance, uint32_t feedrate)
	{
		Comm::DUET.SendGcode("G28\n");
	}

	void ExtrudePresenter::extrude(uint32_t distance, uint32_t feedrate)
	{
		Comm::DUET.SendGcode("G32\n");
	}

	void ExtrudePresenter::newToolData()
	{
		ModelLock lock;
		m_view->setToolCount(OM::GetToolCount());
	}
} // namespace UI
