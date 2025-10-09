/*
 * ToolListPresenter.cpp
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#include "ToolListPresenter.h"
#include "Configuration.h"
#include "Debug.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Tool.h"
#include "ToolList.h"
#include "UI/Core/Model.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	void ToolListPresenter::update()
	{
		MODEL_LOCK();
		size_t added_items = 0;

		getView()->setToolCount(OM::GetToolCount());

		added_items = getView()->setBedCount(OM::GetBedCount());

		/* Set event callbacks for new beds */
		for (size_t i = getView()->getBedCount() - added_items; i < getView()->getBedCount(); i++)
		{
			auto item = getView()->getBed(i);
			if (item == nullptr)
			{
				continue;
			}
			item->setStatusCallback(
				[this, i]()
				{
					auto bed = OM::GetBedBySlot(i);
					if (bed == nullptr)
						return;

					bed->ToggleBedState();
				});
			item->setTemperatureCallback(
				[this, i](bool isActive)
				{
					auto bed = OM::GetBedBySlot(i);
					if (bed == nullptr)
						return;

					NumberPadConfig config = {.header =
												  fmt::format(fmt::runtime(_("tool_list_numpad_header_bed_chamber")),
															  _("bed"),
															  i,
															  isActive ? _("active") : _("standby")),
											  .initialValue = isActive ? bed->GetActiveTemp() : bed->GetStandbyTemp(),
											  .min = bed->GetMin(),
											  .max = bed->GetMax(),
											  .confirmCb = [i, isActive](float value)
											  {
												  auto bed = OM::GetBedBySlot(i);
												  if (bed == nullptr)
													  return;
												  bed->SetBedTemp((int32_t)value, isActive);
											  }};

					configureNumberPad(config);
					getView()->showNumberPad();
				});
		}

		added_items = getView()->setChamberCount(OM::GetChamberCount());

		/* Set event callbacks for new chambers */
		for (size_t i = getView()->getChamberCount() - added_items; i < getView()->getChamberCount(); i++)
		{
			auto item = getView()->getChamber(i);
			if (item == nullptr)
			{
				continue;
			}
			item->setStatusCallback(
				[this, i]()
				{
					auto chamber = OM::GetChamberBySlot(i);
					if (chamber == nullptr)
						return;

					chamber->ToggleChamberState();
				});
			item->setTemperatureCallback(
				[this, i](bool isActive)
				{
					auto chamber = OM::GetChamberBySlot(i);
					if (chamber == nullptr)
						return;

					NumberPadConfig config = {
						.header = fmt::format(fmt::runtime(_("tool_list_numpad_header_bed_chamber")),
											  _("chamber"),
											  i,
											  isActive ? _("active") : _("standby")),
						.initialValue = isActive ? chamber->GetActiveTemp() : chamber->GetStandbyTemp(),
						.min = chamber->GetMin(),
						.max = chamber->GetMax(),
						.confirmCb = [i, isActive](float value)
						{
							auto chamber = OM::GetChamberBySlot(i);
							if (chamber == nullptr)
								return;
							chamber->SetChamberTemp((int32_t)value, isActive);
						}};

					configureNumberPad(config);
					getView()->showNumberPad();
				});
		}

		for (size_t i = 0; i < getView()->getBedCount(); i++)
		{
			auto bed = OM::GetBedBySlot(i);
			auto item = getView()->getBed(i);
			if (bed == nullptr || item == nullptr)
			{
				continue;
			}

			item->setLabel(OM::GetBedCount() > 1 ? fmt::format("{:s} {:d}", _("bed"), bed->index) : _("bed"));
			item->setStatus(_(bed->GetHeaterStatusStr()));
			item->setCurrentTemp(bed->GetCurrentTemp());
			item->setActiveTemp(bed->GetActiveTemp());
			item->setStandbyTemp(bed->GetStandbyTemp());
		}

		for (size_t i = 0; i < getView()->getChamberCount(); i++)
		{
			auto chamber = OM::GetChamberBySlot(i);
			auto item = getView()->getChamber(i);
			if (chamber == nullptr || item == nullptr)
			{
				continue;
			}

			item->setLabel(OM::GetChamberCount() > 1 ? fmt::format("{:s} {:d}", _("chamber"), chamber->index)
													 : _("chamber"));
			item->setStatus(_(chamber->GetHeaterStatusStr()));
			item->setCurrentTemp(chamber->GetCurrentTemp());
			item->setActiveTemp(chamber->GetActiveTemp());
			item->setStandbyTemp(chamber->GetStandbyTemp());
		}
	}

	void ToolListPresenter::onActivate()
	{
		update();
	}

	void ToolListPresenter::onDeactivate() {}

	size_t ToolListToolPresenter::getSlotIndex() const
	{
		return getView()->getIndex();
	}

	void ToolListPresenter::configureNumberPad(const ToolListPresenter::NumberPadConfig& config)
	{
		auto& np = getView()->getNumberPad();
		np.setHeader(config.header);
		np.setMinValue(config.min);
		np.setMaxValue(config.max);
		np.setConfirmCallback(config.confirmCb);
	}

	void ToolListToolPresenter::update()
	{
		size_t slot = getSlotIndex();
		LOG_VERBOSE("Tool index {:d}", slot);

		uint8_t count = 0;
		uint8_t tHeaterIndex = 0;
		m_tool = OM::GetToolBySlot(slot);

		if (m_tool == nullptr)
		{
			LOG_WARN("List index {:d}: Tool is null", slot);
			return;
		}

		getView()->setSelected(m_tool->status == OM::ToolStatus::active);
		getView()->setLabel(m_tool->GetName());
		size_t added_heaters = getView()->setHeaterCount(m_tool->GetHeaterCount());

		/* Set event callbacks for new heaters */
		for (size_t i = getView()->getHeaterCount() - added_heaters; i < getView()->getHeaterCount(); i++)
		{
			auto item = getView()->getHeater(i);
			if (item == nullptr)
			{
				continue;
			}
			item->setStatusCallback(
				[this, i]()
				{
					if (m_tool == nullptr)
						return;

					m_tool->ToggleHeaterState(i);
				});
			item->setTemperatureCallback(
				[this, i](bool isActive)
				{
					if (m_tool == nullptr)
						return;

					auto tHeater = m_tool->GetHeater(i);
					if (tHeater == nullptr)
						return;

					ToolListPresenter::NumberPadConfig config = {
						.header = fmt::format(fmt::runtime(_("tool_list_numpad_header_tool_heater")),
											  m_tool->index,
											  i,
											  isActive ? _("active") : _("standby")),
						.initialValue = isActive ? tHeater->activeTemp : tHeater->standbyTemp,
						.min = tHeater->heater->min,
						.max = tHeater->heater->max,
						.confirmCb = [this, i, isActive](float value)
						{
							if (m_tool == nullptr)
								return;
							m_tool->SetHeaterTemps(i, (int32_t)value, isActive);
						}};

					auto tl_presenter = m_toolListPresenter.lock();
					if (tl_presenter)
					{
						tl_presenter->configureNumberPad(config);
						tl_presenter->getView()->showNumberPad();
					}
				});
		}

		for (size_t i = 0; i < m_tool->GetHeaterCount(); i++)
		{
			auto tHeater = m_tool->GetHeater(i);
			auto item = getView()->getHeater(i);
			if (tHeater == nullptr || item == nullptr)
			{
				continue;
			}

			item->setLabel(tHeater->heater->GetName().length() == 0 ? fmt::format(fmt::runtime(_("heater")), i)
																	: tHeater->heater->GetName());

			item->setStatus(_(tHeater->heater->GetHeaterStatusStr()));
			item->setCurrentTemp(tHeater->heater->current);
			item->setActiveTemp(tHeater->activeTemp);
			item->setStandbyTemp(tHeater->standbyTemp);
		}
	}

	void ToolListToolPresenter::toggleState()
	{
		if (m_tool == nullptr)
		{
			LOG_ERROR("Tool is null");
			return;
		}
		m_tool->ToggleState();
	}

	void ToolListToolPresenter::onActivate()
	{
		update();
	}
} // namespace UI
