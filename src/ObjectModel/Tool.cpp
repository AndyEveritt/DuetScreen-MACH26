/*
 * Tool.cpp
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel & Andy Everitt
 */

#include "Tool.h"

#include "Configuration.h"
#include "Hardware/Duet.h"
#include "ListHelpers.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Utils.h"
#include <Duet3D/General/String.h>
#include <Duet3D/General/Vector.h>

#include "Debug.h"

typedef Vector<std::shared_ptr<OM::Tool>, MAX_SLOTS> ToolList;
static ToolList s_tools;

namespace OM
{
	static int32_t s_currentTool = -1;

	void ToolHeater::Reset()
	{
		index = 0;
		activeTemp = 0;
		standbyTemp = 0;
	}

	void Tool::operator delete(void* p) noexcept
	{
		Tool* t = static_cast<Tool*>(p);
		FreelistManager::Release<Tool>(p);
	}

	std::shared_ptr<ToolHeater> Tool::GetHeater(const uint8_t toolHeaterIndex)
	{
		if (toolHeaterIndex >= MAX_HEATERS_PER_TOOL)
		{
			return nullptr;
		}
		return heaters[toolHeaterIndex];
	}

	std::shared_ptr<ToolHeater> Tool::GetOrCreateHeater(const uint8_t toolHeaterIndex, const uint8_t heaterIndex)
	{
		auto th = GetHeater(toolHeaterIndex);
		if (th != nullptr && th->heater->index == heaterIndex)
		{
			return th;
		}
		if (th == nullptr)
		{
			th = std::make_shared<ToolHeater>();
		}
		th->Reset();
		auto heater = Heat::GetOrCreateHeater(heaterIndex);
		th->index = toolHeaterIndex;
		th->heater = heater;
		LOG_DBG("Setting tool {:d} heater {:d}={:d}", index, toolHeaterIndex, heaterIndex);
		heaters[toolHeaterIndex] = th;
		return th;
	}

	std::shared_ptr<Move::ExtruderAxis> Tool::GetExtruder(const uint8_t toolExtruderIndex) const
	{
		if (toolExtruderIndex >= MAX_EXTRUDERS_PER_TOOL || toolExtruderIndex < 0)
		{
			return nullptr;
		}
		return extruders[toolExtruderIndex];
	}

	std::shared_ptr<Move::ExtruderAxis> Tool::GetOrCreateExtruder(const uint8_t toolExtruderIndex,
																  const uint8_t extruderIndex)
	{
		auto extruder = GetExtruder(toolExtruderIndex);
		if (extruder != nullptr && extruder->index == extruderIndex)
		{
			return extruder;
		}
		extruder = Move::GetOrCreateExtruderAxis(extruderIndex);
		LOG_DBG("Setting tool {:d} extruder {:d}={:d}", index, toolExtruderIndex, extruderIndex);
		extruders[toolExtruderIndex] = extruder;
		return extruder;
	}

	std::shared_ptr<Fan> Tool::GetFan(const uint8_t toolFanIndex)
	{
		if (toolFanIndex >= MAX_FANS)
		{
			return nullptr;
		}
		return fans[toolFanIndex];
	}

	std::shared_ptr<Fan> Tool::GetOrCreateFan(const uint8_t toolFanIndex, const uint8_t fanIndex)
	{
		auto fan = GetFan(toolFanIndex);
		if (fan != nullptr && fan->index == fanIndex)
		{
			return fan;
		}
		fan = OM::GetOrCreateFan(fanIndex);
		LOG_DBG("Setting tool {:d} fan {:d}={:d}", index, toolFanIndex, fanIndex);
		fans[toolFanIndex] = fan;
		return fan;
	}

	StringRef Tool::GetFilament() const
	{
		auto extruder = Move::GetExtruderAxis(filamentExtruder);
		if (extruder == nullptr)
		{
			static String<1> empty;
			return empty.GetRef();
		}
		return extruder->filamentName.GetRef();
	}

	int32_t Tool::GetHeaterTarget(const uint8_t toolHeaterIndex, const bool active)
	{
		auto heater = GetHeater(toolHeaterIndex);
		if (heater == nullptr)
		{
			return -2000;
		}
		return active ? heater->activeTemp : heater->standbyTemp;
	}

	bool Tool::GetHeaterTemps(const StringRef& ref, const bool active)
	{
		for (size_t i = 0; i < MAX_HEATERS_PER_TOOL && heaters[i] != nullptr; ++i)
		{
			if (i > 0)
			{
				ref.cat(':');
			}
			ref.catf("%d", (active ? heaters[i]->activeTemp : heaters[i]->standbyTemp));
		}

		return !ref.IsEmpty();
	}

	bool Tool::SetHeaterTemps(const size_t toolHeaterIndex, const int32_t temp, const bool active)
	{
		String<MAX_COMMAND_LENGTH> command;

		for (size_t i = 0; i < MAX_HEATERS_PER_TOOL && heaters[i] != nullptr; ++i)
		{
			if (i > 0)
			{
				command.cat(':');
			}
			if (i == toolHeaterIndex)
			{
				command.catf("%d", temp);
				continue;
			}
			command.catf("%d", (active ? heaters[i]->activeTemp : heaters[i]->standbyTemp));
		}
		if (command.IsEmpty())
			return false;

		Comm::DUET.SendGcodef("M568 P%d %s%s", index, active ? "S" : "R", command.c_str());

		return true;
	}

	uint8_t Tool::GetHeaterCount() const
	{
		uint8_t count;
		for (count = 0; count < MAX_HEATERS_PER_TOOL && heaters[count] != nullptr; ++count)
		{
		}
		return count;
	}

	int8_t Tool::HasHeater(const uint8_t heaterIndex) const
	{
		for (size_t i = 0; i < MAX_HEATERS_PER_TOOL && heaters[i] != nullptr; ++i)
		{
			if (heaters[i]->heater->index == (int)heaterIndex)
			{
				return i;
			}
		}
		return -1;
	}

	void Tool::IterateHeaters(function_ref<void(std::shared_ptr<ToolHeater>, size_t)> func, const size_t startAt)
	{
		for (size_t i = startAt; i < MAX_HEATERS_PER_TOOL && heaters[i] != nullptr; ++i)
		{
			func(heaters[i], i);
		}
	}

	void Tool::IterateExtruders(function_ref<void(std::shared_ptr<Move::ExtruderAxis>, size_t)> func,
								const size_t startAt)
	{
		for (size_t i = startAt; i < MAX_EXTRUDERS_PER_TOOL && extruders[i] != nullptr; ++i)
		{
			func(extruders[i], i);
		}
	}

	void Tool::IterateFans(function_ref<void(std::shared_ptr<Fan>, size_t)> func, const size_t startAt)
	{
		for (size_t i = startAt; i < MAX_FANS && fans[i] != nullptr; ++i)
		{
			func(fans[i], i);
		}
	}

	size_t Tool::RemoveHeatersFrom(const uint8_t heaterIndex)
	{
		if (heaterIndex >= MAX_HEATERS_PER_TOOL)
		{
			return 0;
		}
		size_t removed = 0;
		for (size_t i = heaterIndex; i < MAX_HEATERS_PER_TOOL && heaters[i] != nullptr; ++i)
		{
			heaters[i].reset();
			++removed;
		}
		return removed;
	}

	size_t Tool::RemoveExtrudersFrom(const uint8_t extruderIndex)
	{
		if (extruderIndex >= MAX_EXTRUDERS_PER_TOOL)
		{
			return 0;
		}
		size_t removed = 0;
		for (size_t i = extruderIndex; i < MAX_EXTRUDERS_PER_TOOL && extruders[i] != nullptr; ++i)
		{
			extruders[i].reset();
			++removed;
		}
		return removed;
	}

	size_t Tool::RemoveFansFrom(const uint8_t fanIndex)
	{
		if (fanIndex >= MAX_FANS)
		{
			return 0;
		}
		size_t removed = 0;
		for (size_t i = fanIndex; i < MAX_FANS && fans[i] != nullptr; ++i)
		{
			fans[i].reset();
			++removed;
		}
		return removed;
	}

	void Tool::UpdateTemp(const uint8_t toolHeaterIndex, const int32_t temp, const bool active)
	{
		auto toolHeater = GetHeater(toolHeaterIndex);
		if (toolHeater == nullptr)
		{
			return;
		}
		if (active)
		{
			toolHeater->activeTemp = temp;
		}
		else
		{
			toolHeater->standbyTemp = temp;
		}
	}

	void Tool::ToggleState()
	{
		switch (status)
		{
		case ToolStatus::active:
			Comm::DUET.SendGcode("T-1");
			break;
		case ToolStatus::standby:
		case ToolStatus::off:
			Comm::DUET.SendGcodef("T%d", index);
			break;
		}
	}

	void Tool::ToggleHeaterState(const uint8_t toolHeaterIndex)
	{
		auto toolHeater = GetHeater(toolHeaterIndex);
		if (toolHeater == nullptr)
		{
			return;
		}
		switch (toolHeater->heater->status)
		{
		case Heat::HeaterStatus::active:
			Comm::DUET.SendGcodef("M568 P%d A1", index);
			break;
		case Heat::HeaterStatus::fault:
			Comm::DUET.SendGcodef("M562 P%d", toolHeater->heater->index);
			break;
		case Heat::HeaterStatus::off:
			Comm::DUET.SendGcodef("M568 P%d A2", index);
			break;
		case Heat::HeaterStatus::standby:
			Comm::DUET.SendGcodef("M568 P%d A0", index);
			break;
		case Heat::HeaterStatus::offline:
		case Heat::HeaterStatus::tuning:
			break;
		}
	}

	void Tool::ToggleSpindleState()
	{
		if (spindle == nullptr)
		{
			return;
		}
		switch (spindle->state)
		{
		case SpindleState::forward:
		case SpindleState::reverse:
			Comm::DUET.SendGcodef("M5");
			break;
		case SpindleState::stopped:
			Comm::DUET.SendGcodef("M3");
			break;
		}
	}

	void Tool::UpdateSpindleTarget(const int32_t rpm)
	{
		if (spindle == nullptr)
		{
			return;
		}
		Comm::DUET.SendGcodef("M568 P%d F%d", index, rpm);
	}

	void Tool::ChangeFilament(const char* filament)
	{
		if (filamentExtruder < 0)
		{
			LOG_WARN("No filament extruder assigned to tool {:d}", index);
			return;
		}
		auto extruder = Move::GetExtruderAxis(filamentExtruder);
		if (extruder == nullptr)
		{
			LOG_WARN("Failed to get extruder {:d} for tool {:d}", filamentExtruder, index);
			return;
		}
		if (extruder->filamentName.Equals(filament))
		{
			return;
		}

		if (filament[0] == '\0')
		{
			UnloadFilament();
			return;
		}

		std::string command = utils::format(
			"T%u\n%sM701 S\"%s\"\nM703\n", index, extruder->filamentName.IsEmpty() ? "" : "M702\n", filament);

		Comm::DUET.SendGcode(command.c_str());
	}

	void Tool::LoadFilament(const char* filament)
	{
		if (filamentExtruder < 0)
		{
			LOG_WARN("No filament extruder assigned to tool {:d}", index);
			return;
		}
		auto extruder = Move::GetExtruderAxis(filamentExtruder);
		if (extruder == nullptr)
		{
			LOG_WARN("Failed to get extruder {:d} for tool {:d}", filamentExtruder, index);
			return;
		}
		if (extruder->filamentName.Equals(filament))
		{
			return;
		}

		if (filament[0] == '\0')
		{
			return;
		}

		Comm::DUET.SendGcodef("T%d M701 S\"%s\"\n", index, filament);
	}

	void Tool::UnloadFilament()
	{
		if (filamentExtruder < 0)
		{
			LOG_WARN("No filament extruder assigned to tool {:d}", index);
			return;
		}
		auto extruder = Move::GetExtruderAxis(filamentExtruder);
		if (extruder == nullptr)
		{
			LOG_WARN("Failed to get extruder {:d} for tool {:d}", filamentExtruder, index);
			return;
		}
		if (extruder->filamentName.IsEmpty())
		{
			return;
		}

		Comm::DUET.SendGcodef("T%d M702\n", index);
	}

	void Tool::Reset()
	{
		index = 0;
		for (size_t i = 0; i < MAX_HEATERS_PER_TOOL; ++i)
		{
			heaters[i].reset();
		}
		for (size_t i = 0; i < MAX_EXTRUDERS_PER_TOOL; ++i)
		{
			extruders[i].reset();
		}
		for (size_t i = 0; i < MAX_FANS; ++i)
		{
			fans[i].reset();
		}
		filamentExtruder = -1;
		spindle.reset();
		spindleRpm = 0;
		for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
		{
			offsets[i] = 0.0f;
		}
		for (size_t i = 0; i < MAX_EXTRUDERS_PER_TOOL; ++i)
		{
			mix[i] = 0.0f;
		}
		status = ToolStatus::off;
	}

	std::shared_ptr<Tool> GetTool(const size_t index)
	{
		return GetOrCreate<ToolList, Tool>(s_tools, index, false);
	}

	std::shared_ptr<Tool> GetOrCreateTool(const size_t index)
	{
		LOG_DBG("{:d}", index);
		return GetOrCreate<ToolList, Tool>(s_tools, index, true);
	}

	std::shared_ptr<Tool> GetToolBySlot(const size_t slot)
	{
		if (slot >= s_tools.Size())
		{
			return nullptr;
		}
		return s_tools[slot];
	}

	const size_t GetToolCount()
	{
		return s_tools.Size();
	}

	bool IterateToolsWhile(function_ref<bool(std::shared_ptr<Tool>, size_t)> func, const size_t startAt)
	{
		return s_tools.IterateWhile(func, startAt);
	}

	size_t RemoveTool(const size_t index, const bool allFollowing)
	{
		LOG_DBG("Removing tool {:d} (allFollowing={:s})", index, allFollowing ? "true" : "false");
		return Remove<ToolList, Tool>(s_tools, index, allFollowing);
	}

	bool UpdateToolHeater(const size_t toolIndex, const size_t toolHeaterIndex, const uint8_t heaterIndex)
	{
		if (toolHeaterIndex >= MAX_HEATERS_PER_TOOL)
		{
			return false;
		}
		auto tool = OM::GetOrCreateTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		auto heater = tool->GetOrCreateHeater(toolHeaterIndex, heaterIndex);
		if (heater == nullptr)
		{
			LOG_ERROR("Failed to get or create tool {:d} heater {:d}={:d}", toolIndex, toolHeaterIndex, heaterIndex);
			return false;
		}
		LOG_DBG("Assigned heater {:d} to tool {:d} heaterIndex {:d}", heaterIndex, toolIndex, toolHeaterIndex);
		return true;
	}

	bool RemoveToolHeaters(const size_t toolIndex, const uint8_t firstIndexToDelete)
	{
		auto tool = OM::GetTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		return tool->RemoveHeatersFrom(firstIndexToDelete) > 0;
	}

	bool UpdateToolExtruder(const size_t toolIndex, const size_t toolExtruderIndex, const uint8_t extruderIndex)
	{
		if (toolExtruderIndex >= MAX_EXTRUDERS_PER_TOOL)
		{
			return false;
		}
		auto tool = OM::GetOrCreateTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		auto extruder = tool->GetOrCreateExtruder(toolExtruderIndex, extruderIndex);
		if (extruder == nullptr)
		{
			LOG_ERROR(
				"Failed to get or create tool {:d} extruder {:d}={:d}", toolIndex, toolExtruderIndex, extruderIndex);
			return false;
		}
		return true;
	}

	bool RemoveToolExtruders(const size_t toolIndex, const uint8_t firstIndexToDelete)
	{
		auto tool = OM::GetTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		return tool->RemoveExtrudersFrom(firstIndexToDelete) > 0;
	}

	bool UpdateToolMix(const size_t toolIndex, const size_t toolExtruderIndex, const float mix)
	{
		auto tool = OM::GetOrCreateTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}

		tool->mix[toolExtruderIndex] = mix;
		return true;
	}

	bool UpdateToolFan(const size_t toolIndex, const size_t toolFanIndex, const uint8_t fanIndex)
	{
		if (toolFanIndex >= MAX_FANS)
		{
			return false;
		}
		auto tool = OM::GetOrCreateTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		auto fan = tool->GetOrCreateFan(toolFanIndex, fanIndex);
		if (fan == nullptr)
		{
			LOG_ERROR("Failed to get or create tool {:d} fan {:d}={:d}", toolIndex, toolFanIndex, fanIndex);
			return false;
		}
		LOG_DBG("Assigned fan {:d} to tool {:d} fanIndex {:d}", fanIndex, toolIndex, toolFanIndex);
		return true;
	}

	bool UpdateToolFilamentExtruder(const size_t toolIndex, const int8_t extruderIndex)
	{
		auto tool = OM::GetOrCreateTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		tool->filamentExtruder = extruderIndex;
		return true;
	}

	bool RemoveToolFans(const size_t toolIndex, const uint8_t firstIndexToDelete)
	{
		auto tool = OM::GetTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		return tool->RemoveFansFrom(firstIndexToDelete) > 0;
	}

	bool UpdateToolTemp(const size_t toolIndex, const size_t toolHeaterIndex, const int32_t temp, const bool active)
	{
		auto tool = OM::GetOrCreateTool(toolIndex);

		// If we do not handle this tool back off
		if (tool == nullptr)
		{
			return false;
		}

		tool->UpdateTemp(toolHeaterIndex, temp, active);
		return true;
	}

	bool UpdateToolName(const size_t toolIndex, const char* name)
	{
		auto tool = OM::GetOrCreateTool(toolIndex);

		// If we do not handle this tool back off
		if (tool == nullptr)
		{
			return false;
		}

		tool->name.copy(name, MAX_TOOL_NAME_LENGTH);
		LOG_DBG("Tool {:d} name={:s}", tool->index, tool->name.c_str());
		return true;
	}

	bool UpdateToolStatus(const size_t toolIndex, const char* statusStr)
	{
		auto tool = OM::GetOrCreateTool(toolIndex);

		// If we do not handle this tool back off
		if (tool == nullptr)
		{
			return false;
		}

		const ToolStatusMapEntry key = {statusStr, ToolStatus::off};
		const ToolStatusMapEntry* statusFromMap = (ToolStatusMapEntry*)bsearch(
			&key, toolStatusMap, ARRAY_SIZE(toolStatusMap), sizeof(ToolStatusMapEntry), compareKey<ToolStatusMapEntry>);
		ToolStatus status = (statusFromMap != nullptr) ? statusFromMap->val : ToolStatus::off;
		tool->status = status;
		return true;
	}

	bool UpdateToolSpindle(const size_t toolIndex, const int8_t spindleIndex)
	{
		auto tool = OM::GetOrCreateTool(toolIndex);

		// If we do not handle this tool back off
		if (tool == nullptr)
		{
			return false;
		}

		tool->spindle = (spindleIndex < 0) ? nullptr : OM::GetOrCreateSpindle(spindleIndex);
		return true;
	}

	bool UpdateToolSpindleRpm(const size_t toolIndex, const int32_t rpm)
	{
		auto tool = OM::GetOrCreateTool(toolIndex);

		// If we do not handle this tool back off
		if (tool == nullptr)
		{
			return false;
		}

		tool->spindleRpm = rpm;
		return true;
	}

	void SetCurrentTool(const int32_t toolIndex)
	{
		LOG_DBG("Setting current tool to {:d}", toolIndex);
		s_currentTool = toolIndex;
	}

	std::shared_ptr<Tool> GetCurrentTool()
	{
		if (s_currentTool < 0)
		{
			LOG_DBG("No tool selected");
			return nullptr;
		}
		return GetTool(s_currentTool);
	}
} // namespace OM
