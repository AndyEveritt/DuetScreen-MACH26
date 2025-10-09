/*
 * BedOrChamber.h
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#ifndef SRC_OBJECTMODEL_BEDORCHAMBER_HPP_
#define SRC_OBJECTMODEL_BEDORCHAMBER_HPP_

#include "ObjectModel/Heat.h"
#include <Duet3D/General/FreelistManager.h>
#include <Duet3D/General/function_ref.h>
#include <cstdint>
#include <memory>
#include <sys/types.h>

namespace OM
{
	struct BedOrChamber
	{
		void* operator new(size_t) noexcept { return FreelistManager::Allocate<BedOrChamber>(); }
		void operator delete(void* p) noexcept { FreelistManager::Release<BedOrChamber>(p); }

		// Index within configured heaters
		uint8_t index;
		// Id of heater
		int8_t heater;
		// Slot for display on panel
		uint8_t slot;

		void Reset();
		float GetCurrentTemp();
		int32_t GetCurrentTarget();
		int32_t GetActiveTemp();
		int32_t GetStandbyTemp();
		float GetMin();
		float GetMax();
		bool SetBedTemp(const int32_t temp, const bool active);
		bool SetChamberTemp(const int32_t temp, const bool active);
		bool ToggleBedState();
		bool ToggleChamberState();
		Heat::HeaterStatus GetHeaterStatus();
		const char* GetHeaterStatusStr();
	};

	using Bed = BedOrChamber;
	using Chamber = BedOrChamber;
	using BedPtr = std::shared_ptr<Bed>;
	using ChamberPtr = std::shared_ptr<Chamber>;
	using BedOrChamberPtr = std::shared_ptr<BedOrChamber>;

	BedPtr GetBedBySlot(const size_t index);
	BedPtr GetBed(const size_t index);
	BedPtr GetOrCreateBed(const size_t index);
	BedPtr GetFirstBed();
	size_t GetBedCount();
	bool IterateBedsWhile(function_ref<bool(BedPtr, size_t)> func, const size_t startAt = 0);
	size_t RemoveBed(const size_t index, const bool allFollowing);

	ChamberPtr GetChamberBySlot(const size_t index);
	ChamberPtr GetChamber(const size_t index);
	ChamberPtr GetOrCreateChamber(const size_t index);
	ChamberPtr GetFirstChamber();
	size_t GetChamberCount();
	bool IterateChambersWhile(function_ref<bool(ChamberPtr, size_t)> func, const size_t startAt = 0);
	size_t RemoveChamber(const size_t index, const bool allFollowing);
	bool SetBedHeater(const uint8_t bedIndex, const int8_t heaterNumber);
	bool SetChamberHeater(const uint8_t chamberIndex, const int8_t heaterNumber);

	extern int8_t g_lastBed;
	extern int8_t g_lastChamber;
} // namespace OM

#endif /* SRC_OBJECTMODEL_BEDORCHAMBER_HPP_ */
