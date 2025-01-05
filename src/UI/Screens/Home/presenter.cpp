#include "presenter.h"
#include "view.h"

#include "ObjectModel/Heat.h"

namespace UI
{

	void HomePresenter::newHeaterData()
	{
		OM::Heat::Heater* heater = OM::Heat::GetHeater(1);
		if (heater == nullptr)
		{
			return;
		}
		m_view->setHeaterData(heater->GetHeaterStatusStr());
	}

} // namespace UI
