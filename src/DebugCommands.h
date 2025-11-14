/*
 * DebugCommands.h
 *
 *  Created on: 14 Mar 2024
 *      Author: andy
 */

#ifndef JNI_DEBUGCOMMANDS_H_
#define JNI_DEBUGCOMMANDS_H_

#include <functional>

namespace Debug
{
	typedef std::function<void(void)> debugCb_t;

	struct DebugCommand
	{
		const char* m_id;
		debugCb_t m_callback;

		DebugCommand(const char* id, debugCb_t callback);
	};

	void CreateCommand(const char* id, debugCb_t callback);
	DebugCommand* GetCommand(const char* id);
	DebugCommand* GetCommandByIndex(size_t index);
	size_t GetCommandCount();
} // namespace Debug

#endif /* JNI_DEBUGCOMMANDS_H_ */
