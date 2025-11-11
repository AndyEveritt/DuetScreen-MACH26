/*
 * ExtrusionFactorPresenter.cpp
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#include "ExtrusionFactorPresenter.h"
#include "Debug.h"
#include "ExtrusionFactor.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/Tool.h"
#include "i18n/i18n.h"

namespace UI
{
	void ExtrusionFactorPresenter::newExtruderData()
	{
		m_view->setExtruderCount(OM::Move::GetExtruderAxisCount());

		OM::Move::IterateExtruderAxesWhile(
			[this](std::shared_ptr<OM::Move::ExtruderAxis> extruder, size_t index)
			{
				m_view->setExtruderLabel(index, _("fine_tune.extruder", extruder->index));
				m_view->setExtruderValue(index, std::round(100 * extruder->factor));
				return true;
			});
	}

	void ExtrusionFactorPresenter::setExtruderFactor(size_t slot, uint32_t value)
	{
		auto extruder = OM::Move::GetExtruderAxisBySlot(slot);
		if (extruder == nullptr || value == std::round(100 * extruder->factor))
		{
			return;
		}

		LOG_DBG("Setting extruder {:d} slider factor to {:d}", extruder->index, value);
		Comm::DUET.SendGcodef("M221 D{:d} S{:d}\n", extruder->index, value);
	}

	void ModalExtrusionFactorPresenter::configureNumberPad(size_t index)
	{
		auto extruder = OM::Move::GetExtruderAxisBySlot(index);
		configureNumberPad(std::move(extruder));
	};

	void ModalExtrusionFactorPresenter::configureNumberPad(OM::Move::ExtruderAxisPtr extruder)
	{
		auto& np = m_view->getNumberPad();

		np.setVisible(extruder != nullptr);

		if (!extruder)
		{
			LOG_ERROR("No extruder found for number pad configuration");
			return;
		}

		/** TODO this is effectively duplicated from ExtrusionFactor::configureNumberPad(Slider* slider) but because
		 * we need to configure the number pad on activation, we don't have a slider to pass to it.
		 */

		np.setHeader(_("fine_tune.extruder", extruder->index));
		np.setValue(std::round(100 * extruder->factor));
		np.setMinValue(0);
		np.setConfirmCallback(
			[index = extruder->index](float value)
			{
				auto extruder = OM::Move::GetExtruderAxis(index);
				if (extruder == nullptr || value == std::round(100 * extruder->factor))
				{
					return;
				}

				LOG_DBG("Setting extruder {:d} number pad factor to {:g}", extruder->index, value);
				Comm::DUET.SendGcodef("M221 D{:d} S{:g}\n", extruder->index, value);
			});
	}

	void ModalExtrusionFactorPresenter::onActivate()
	{
		auto tool = OM::GetCurrentTool();
		OM::Move::ExtruderAxisPtr extruder;
		if (tool)
		{
			extruder = tool->GetExtruder(0);
		}

		configureNumberPad(std::move(extruder));
	}
} // namespace UI
