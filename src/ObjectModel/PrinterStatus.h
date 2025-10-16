/*
 * PrinterStatus.h
 *
 *  Created on: 6 Jan 2017
 *      Author: David
 */

#ifndef SRC_OBJECTMODEL_PRINTERSTATUS_HPP_
#define SRC_OBJECTMODEL_PRINTERSTATUS_HPP_

#include <cstdint>
#include <string>

namespace OM
{
	// Status that the printer may report to us.
	// Must be the same order as printerStatusMap.
	enum class PrinterStatus
	{
		unknown = -1,
		busy,
		cancelling,
		changingTool,
		connecting,
		halted,
		idle,
		off,
		paused,
		pausing,
		processing,
		resuming,
		simulating,
		starting,
		updating,
		NumTypes
	};

	struct PrinterStatusMapEntry
	{
		const char* key;
		PrinterStatus val;
	};

	// This table must be kept in case-insensitive alphabetical order of the search string.
	const PrinterStatusMapEntry printerStatusMap[] = {
		{"busy", PrinterStatus::busy},
		{"cancelling", PrinterStatus::cancelling},
		{"changingTool", PrinterStatus::changingTool},
		{"connecting", PrinterStatus::connecting},
		{"halted", PrinterStatus::halted},
		{"idle", PrinterStatus::idle},
		{"off", PrinterStatus::off},
		{"paused", PrinterStatus::paused},
		{"pausing", PrinterStatus::pausing},
		{"processing", PrinterStatus::processing},
		{"resuming", PrinterStatus::resuming},
		{"simulating", PrinterStatus::simulating},
		{"starting", PrinterStatus::starting},
		{"updating", PrinterStatus::updating},
	};

	static_assert(std::size(printerStatusMap) == (size_t)PrinterStatus::NumTypes);

	bool IsPrintingStatus(PrinterStatus status);
	bool PrintInProgress();
	bool OkToSend();
	PrinterStatus GetStatus();
	const char* GetStatusText();
	void SetStatus(const char* status);
	void SetStatus(const PrinterStatus status);
	const std::string& GetPrinterName();
	void SetPrinterName(const char* name);

	uint32_t GetChannelIndex();
	void SetChannelIndex(uint32_t index);
} // namespace OM

#endif /* SRC_OBJECTMODEL_PRINTERSTATUS_HPP_ */
